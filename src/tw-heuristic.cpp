#include <atomic>
#include <csignal>
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

namespace {
std::atomic<std::string *> best_td_str(new std::string(""));
std::atomic<size_t> best_width(0);

void signal_handler(int signum) {
  if (signum == SIGUSR1) {
    std::cout << best_width.load() << '\n';
  } else if (signum == SIGTERM) {
    std::cout << *best_td_str.load();
    std::exit(0);
  }
}
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "No arguments provided, aborting\n";
    return 1;
  }

  // Set up signal handling
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGUSR1);
  sigaddset(&sa.sa_mask, SIGTERM);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  int opt;
  while ((opt = getopt(argc, argv, "fds:")) != -1) {
    if (opt == 's') {
      srand(atoi(optarg));
    } else {
      std::cerr << "Invalid argument: " << opt << ", aborting\n";
      return 2;
    }
  }

  if (optind != argc - 1) {
    std::cerr << "Missing filename argument, aborting\n";
    return 3;
  }

  const Graph graph(argv[argc - 1]);

  TD td = minimum_degree_heuristic(graph);

  best_width = td.width();

  std::string *tmp_str = new std::string("");

  *tmp_str = td.to_string(graph);
  tmp_str = best_td_str.exchange(tmp_str);

  while (true) {
    td = minimum_fillin_heuristic(graph, best_width.load());
    if (td.width() < best_width.load()) {
      *tmp_str = td.to_string(graph);
      tmp_str = best_td_str.exchange(tmp_str);
    }
    break;
  }

  delete tmp_str;
  delete best_td_str.load();
}
