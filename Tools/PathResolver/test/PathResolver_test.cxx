/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include <PathResolver/PathResolver.h>

#include <string>
#include <iostream>
using namespace std;

std::string cleanLoc (const std::string& s)
{
  std::string pat = "/PathResolver/run/";
  std::string::size_type pos = s.find (pat);
  if (pos != std::string::npos) {
    return s.substr (pos + pat.size(), std::string::npos);
  }

  pat = "/PathResolver/test/";
  pos = s.find (pat);
  if (pos != std::string::npos) {
    return "../test/" + s.substr (pos + pat.size(), std::string::npos);
  }

  return s;
}


int main ()
{
  cout << "*** PathResolver_test starts ***" << endl;
  std::string filename = "a.txt";
  std::string fileLocation;

  fileLocation = PathResolver::find_file (filename, "DATAPATH", PathResolver::RecursiveSearch);
  std::cout << "filename " << filename << std::endl;
  std::cout << "fileLocation " << cleanLoc(fileLocation) << std::endl;


  filename = "B/a.txt";
  fileLocation = PathResolver::find_file (filename, "DATAPATH");

  std::cout << "filename " << filename << std::endl;
  std::cout << "fileLocation " << cleanLoc(fileLocation) << std::endl;
  cout << "*** PathResolver_test ends ***" << endl;

  return (0);
}
