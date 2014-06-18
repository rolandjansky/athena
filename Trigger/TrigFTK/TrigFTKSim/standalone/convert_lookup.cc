/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKCacheLookup.h"
#include <iostream>
#include <cstdlib>

using namespace std;
   
int main(int argc, char *argv[]){
  if(argc!=3) {
    cerr << "This executable converts root-based region lookup maps to plan text." << endl;
    cerr << "USAGE: " << argv[0] << " /input/lookup_file.root /output/lookup_file.dat" << endl;
    exit(1);
  }
  FTKIntLookup  clu;
  clu.open_for_read(argv[1]);
  cout << "Please wait, this may take a few minutes..." << endl;
  clu.dump_to_textfile(argv[2]);
  return 0;
}
