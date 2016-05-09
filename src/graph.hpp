#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <set>
#include <utility>
#include <vector>

#include "range.hpp"

using Vertex = uint32_t;
using Edge = std::pair<Vertex, Vertex>;

class Graph {
 private:
  size_t num_vertices_;
  std::vector<std::set<Vertex>> adj;

 public:
  using Neighborhood = const std::set<Vertex> &;

  explicit Graph(size_t size) : num_vertices_(size), adj(size) {}

  explicit Graph(const std::string &filename) {
    std::vector<Edge> edges;
    std::ifstream graph_file(filename);
    std::string ignore;
    std::getline(graph_file, ignore);
    Vertex v, w, n = 0;
    while (graph_file >> v >> w) {
      edges.push_back({std::min(v, w) - 1, std::max(v, w) - 1});
      n = std::max(n, std::max(v, w));
    }
    num_vertices_ = n;
    adj.resize(n);
    for (Edge e : edges) {
      add_arc(e.first, e.second);
      add_arc(e.second, e.first);
    }
  }

  size_t num_vertices() const { return num_vertices_; }
  Neighborhood neighbors(Vertex v) const { return adj[v]; }
  size_t degree(Vertex v) const { return adj[v].size(); }
  Range<Vertex> vertices() const { return {Vertex(0), Vertex(num_vertices())}; }
  bool adjacent(Vertex v, Vertex w) const { return adj[v].count(w) > 0; }
  void add_arc(Vertex u, Vertex v) { adj[u].insert(v); }
  void remove_arc(Vertex u, Vertex v) { adj[u].erase(v); }
};
