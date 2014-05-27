void rcu_hadd (unsigned max_files, std::string output_file)
{
  // load the libraries for all packages
  gROOT->ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");

  // read the input files from standard input
  std::vector<std::string> input_files;
  std::string line;
  while (std::getline (std::cin, line))
    input_files.push_back (line);

  RCU::hadd_core (output_file, input_files, max_files);
}
