#pragma once
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

#include "always_assert.hpp"
#include "graph.hpp"

class TD {
 public:
  using Bag = size_t;

  void add_leaf(Vertex v) {
    bags.push_back({v});
    parent.push_back(0);
  }

  Bag add_child(Bag p, std::vector<Vertex> &&contents) {
    always_assert(0 <= p && p < bags.size());
    bags.emplace_back(contents);
    parent.push_back(p);
    return bags.size() - 1;
  }

  size_t width() const {
    size_t max_width = 0;
    for (const auto &bag : bags)
      max_width = std::max(bag.size(), max_width);
    return max_width;
  }

  void print(const Graph &graph, std::ostream &out = std::cout) const {
    out << "s td " << bags.size() << ' ' << width() << ' '
        << graph.num_vertices() << "\n";
    for (size_t i = 0; i < bags.size(); ++i) {
      out << "b " << i + 1 << ' ';
      for (Vertex v : bags[i])
        out << v + 1 << ' ';
      out << "\n";
    }
    for (size_t i = 1; i < bags.size(); ++i)
      out << parent[i] + 1 << ' ' << i + 1 << "\n";
  }

 private:
  using BagImpl = std::vector<Vertex>;
  std::vector<BagImpl> bags;
  std::vector<Bag> parent;
};
