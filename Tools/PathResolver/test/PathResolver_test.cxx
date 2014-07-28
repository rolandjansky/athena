/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include <PathResolver/PathResolver.h>

#include <string>
#include <iostream>
using namespace std;
int main ()
{
  cout << "*** PathResolver_test starts ***" << endl;
  std::string filename = "a.txt";
  std::string fileLocation;

  fileLocation = PathResolver::find_file (filename, "DATAPATH", PathResolver::RecursiveSearch);
  std::cout << "filename " << filename << std::endl;
  std::cout << "fileLocation " << fileLocation << std::endl;


  filename = "B/a.txt";
  fileLocation = PathResolver::find_file (filename, "DATAPATH");

  std::cout << "filename " << filename << std::endl;
  std::cout << "fileLocation " << fileLocation << std::endl;
  cout << "*** PathResolver_test ends ***" << endl;

  return (0);
}
