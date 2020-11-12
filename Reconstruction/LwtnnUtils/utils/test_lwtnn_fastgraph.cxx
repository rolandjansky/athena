#include "LwtnnUtils/FastGraph.h"
#include "LwtnnUtils/InputOrder.h"

#include "lwtnn/lightweight_network_config.hh"
#include "lwtnn/parse_json.hh"

#include <string>
#include <vector>
#include <fstream>

struct Args
{
  std::string nn_file;
};

Args getArgs(int nargs, char* argv[]) {
  Args args;
  if (nargs != 1) return args;
  args.nn_file = argv[1];
  return args;
}

int main(int nargs, char* argv[]) {
  Args args = getArgs(nargs, argv);
  if (args.nn_file.size() == 0) return 1;
  auto nn_file = std::ifstream(args.nn_file);
  auto graph_config = lwt::parse_json_graph(nn_file);

  lwt::atlas::FastGraph graph(graph_config, {}, "");

  return 0;
}
