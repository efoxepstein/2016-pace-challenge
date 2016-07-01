#pragma once
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <forward_list>
#include <set>
#include <utility>
#include <vector>

#include "always_assert.hpp"
#include "range.hpp"

using Vertex = uint32_t;
using VertexList = std::vector<Vertex>;

using Edge = std::pair<Vertex, Vertex>;

class Graph {
 private:
  size_t num_vertices_;

  std::vector<bool> adj;
  std::vector<size_t> deg;
  std::vector<VertexList> adj_list;

  size_t idx(Vertex u, Vertex v) const { return u * num_vertices_ + v; }

 public:
  explicit Graph(const std::string &filename) {
    std::vector<Edge> edges;
    std::ifstream graph_file(filename);

    Vertex v, w, n = 0;

    std::string header_line;
    std::getline(graph_file, header_line);
    for (size_t i = 5; header_line[i] != ' '; ++i) {
      n = n * 10 + (header_line[i] - '0');
    }

    while (graph_file >> v >> w) {
      edges.push_back({std::min(v, w) - 1, std::max(v, w) - 1});
    }

    num_vertices_ = n;

    adj.resize(n * n, false);
    adj_list.resize(n);
    deg.resize(n, 0);

    for (Edge e : edges) {
      add_arc(e.first, e.second);
      add_arc(e.second, e.first);
    }
  }

  size_t num_vertices() const { return num_vertices_; }

  const VertexList &neighbors(Vertex v) {
    auto end = std::remove_if(adj_list[v].begin(), adj_list[v].end(),
                              [v, this](Vertex w) { return !adjacent(v, w); });
    adj_list[v].erase(end, adj_list[v].end());
    return adj_list[v];
  }

  size_t degree(Vertex v) const { return deg[v]; }
  Range<Vertex> vertices() const { return {Vertex(0), Vertex(num_vertices())}; }
  bool adjacent(Vertex v, Vertex w) const { return adj[idx(v, w)]; }
  void add_edge(Vertex u, Vertex v) {
    add_arc(u, v);
    add_arc(v, u);
  }
  void add_arc(Vertex u, Vertex v) {
    if (adj[idx(u, v)])
      return;
    adj[idx(u, v)] = true;
    adj_list[u].push_back(v);
    ++deg[u];
  }
  void remove_edge(Vertex u, Vertex v) {
    remove_arc(u, v);
    remove_arc(v, u);
  }
  void remove_arc(Vertex u, Vertex v) {
    always_assert(adj[idx(u, v)]);
    adj[idx(u, v)] = false;
    always_assert(deg[u] > 0);
    --deg[u];
  }
};
