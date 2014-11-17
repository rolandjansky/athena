#ifndef __CINT__
#include <RootCoreUtils/hadd.h>
#endif

void rcu_hadd (unsigned max_files, std::string output_file)
{
  // read the input files from standard input
  std::vector<std::string> input_files;
  std::string line;
  while (std::getline (std::cin, line))
    input_files.push_back (line);

  RCU::hadd_core (output_file, input_files, max_files);
}
