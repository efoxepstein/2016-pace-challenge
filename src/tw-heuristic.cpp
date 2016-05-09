#include <cstdlib>
#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "always_assert.hpp"
#include "graph.hpp"
#include "minimum_degree_heuristic.hpp"
#include "tree_decomposition.hpp"

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "No arguments provided.\n";
    return 1;
  }

  std::string filename;

  int opt;
  while ((opt = getopt(argc, argv, "fds:")) != -1) {
    if (opt == 's')
      srand(atoi(optarg));
    else
      always_assert(false && "Invalid argument.");
  }

  always_assert(optind == argc - 1);

  Graph g(argv[argc-1]);

  TD td1 = minimum_degree_heuristic(g);
  TD td2 = minimum_fillin_heuristic(g);

  if (td1.width() < td2.width())
    td1.print(g);
  else
    td2.print(g);
}
