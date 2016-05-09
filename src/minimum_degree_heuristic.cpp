#include "minimum_degree_heuristic.hpp"

#include <set>
#include <utility>
#include <vector>

namespace {

using ElimOrder = std::vector<Vertex>;

TD elimination_ordering_to_td(const Graph &graph, ElimOrder order) {
  TD td;
  std::vector<size_t> time_processed(graph.num_vertices(),
                                     std::numeric_limits<size_t>::max());
  std::vector<TD::Bag> bag_of_vertex(graph.num_vertices());

  size_t time = 0;
  for (auto itr = order.crbegin(); itr != order.crend(); ++itr, ++time) {
    Vertex v = *itr;

    time_processed[v] = time;

    std::vector<Vertex> neighbors;
    size_t max_time = 0;
    TD::Bag max_bag;
    for (Vertex w : graph.neighbors(v)) {
      if (time_processed[w] < time) {
        neighbors.push_back(w);
        if (time_processed[w] >= max_time) {
          max_time = time_processed[w], max_bag = bag_of_vertex[w];
        }
      }
    }

    if (neighbors.empty()) {
      td.add_leaf(v);
    } else {
      neighbors.push_back(v);
      TD::Bag bag = td.add_child(max_bag, std::move(neighbors));
      bag_of_vertex[v] = bag;
    }
  }

  return td;
}

void triangulate(Graph &graph, Vertex v) {
  for (Vertex w : graph.neighbors(v))
    graph.remove_arc(w, v);

  for (Vertex w1 : graph.neighbors(v))
    for (Vertex w2 : graph.neighbors(v))
      if (w1 != w2 && !graph.adjacent(w1, w2) && !graph.adjacent(w2, w1))
        graph.add_arc(w1, w2), graph.add_arc(w2, w1);
}

template <class Fn>
TD minimum_x_heuristic(Graph &graph, Fn fn) {
  ElimOrder order;
  order.reserve(graph.num_vertices());

  std::set<std::pair<size_t, Vertex>> pq;

  for (Vertex v : graph.vertices())
    pq.emplace(fn(graph, v), v);

  for (size_t ignore = 0; ignore < graph.num_vertices(); ++ignore) {
    Vertex v = pq.begin()->second;
    pq.erase(pq.begin());

    order.push_back(v);

    for (Vertex w : graph.neighbors(v))
      pq.erase({fn(graph, w), w});

    triangulate(graph, v);

    for (Vertex w : graph.neighbors(v))
      pq.emplace(fn(graph, w), w);
  }

  return elimination_ordering_to_td(graph, order);
}

}  // anonymous namespace

TD minimum_degree_heuristic(const Graph &graph) {
  Graph copy = graph;
  return minimum_x_heuristic(
      copy, [](const Graph &graph, Vertex v) { return graph.degree(v); });
}

TD minimum_fillin_heuristic(const Graph &graph) {
  Graph copy = graph;
  return minimum_x_heuristic(copy, [](const Graph &graph, Vertex v) {
    size_t count = 0;
    for (Vertex w1 : graph.neighbors(v))
      for (Vertex w2 : graph.neighbors(v))
        if (w1 < w2 && graph.adjacent(w1, w2) && graph.adjacent(w2, w1))
          ++count;
    return graph.degree(v) * (graph.degree(v) - 1) / 2 - count;
  });
}
