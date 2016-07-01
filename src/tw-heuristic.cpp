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
std::atomic<size_t> best_width(std::numeric_limits<size_t>::max());
std::atomic<TD *> best_td(new TD());
TD *tmp;
Graph *graph;

void signal_handler(int signum) {
  if (signum == SIGUSR1) {
    std::cout << best_width << '\n';
  } else if (signum == SIGTERM) {
    if (best_td != nullptr) {
      best_td.load()->print(*graph);
      std::cout.flush();
    }
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

  graph = new Graph(argv[argc - 1]);
  tmp = new TD();

  *tmp = minimum_degree_heuristic(*graph);
  best_width = tmp->width();
  best_td.exchange(tmp);

  while (true) {
    always_assert(tmp != nullptr);
    *tmp = minimum_fillin_heuristic(*graph);
    size_t tmp_width = tmp->width();
    if (tmp_width < best_width.load()) {
      tmp = best_td.exchange(tmp);
      best_width = tmp_width;
    }
  }
}
