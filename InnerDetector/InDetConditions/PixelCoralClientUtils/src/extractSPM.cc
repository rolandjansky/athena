/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/CoralClient.hh"
#include "PixelCoralClientUtils/SpecialPixelMap.hh"

#include<string>
#include<cstring>
#include<map>
#include<set>
#include<fstream>
using namespace std;

int main ATLAS_NOT_THREAD_SAFE (int argc, char **argv){ // Thread unsafe const_iterator end is used.

  int iarg = 1;
  bool verbose = false;
  if (argc>iarg && strcmp(argv[iarg],"-v")==0) {
    verbose = true;
    ++iarg;
  }

  if (iarg>=argc) {
    cout << "Usage: extractSPM [-v] anal_id" << endl;
    return 1;
  }

  CAN::SerialNumber_t anal_id = atoi(argv[iarg]);

  cout << "--> Summary for anal_id " << anal_id << endl;

  //
  // connect to analysis data base
  //

  try {

    // Start CORAL client
    const char* tableName = "CALIB_ANAL";
    PixCoralClient coralClient(verbose, coral::ReadOnly, tableName);

    // Show the structure of the master table
    if (verbose) {
      coralClient.printTableDesc(tableName);
      coralClient.printTableContent(tableName);
    }

    // Return a CAN::AnalysisResultList_t object
    CAN::AnalysisResultList_t a = coralClient.getAnalysisResultsFromDB(anal_id);

    // load pixel maps
    int npm = 0;
    for (map<string, map<string, PixelMap_t> >::const_iterator iter = a.begin<PixelMap_t>();
	 iter != a.end<PixelMap_t>(); ++iter) {
      ++npm;
      const string& s = iter->first;
      if (verbose) cout << s << endl;
      for (map<string, PixelMap_t>::const_iterator val_iter = iter->second.begin();
	   val_iter != iter->second.end(); ++val_iter) {
	const string& ss = val_iter->first;
	PixelMap_t value = val_iter->second;
	if (verbose) {
	  cout << ss << endl;
	  for (PixelMap_t::const_iterator pm = value.begin();
	       pm != value.end(); ++pm) {
	    cout << "col=" << pm->first.first << ", row=" << pm->first.second << ", status=" << pm->second << endl;
	  }
	}
      }
    }
    cout << "found pixel maps: " << npm << endl;

  } catch ( exception& e ) {
    cout << "exception caught: " << e.what() << endl;
    return -1;
  }

  catch (...) {
    cout << "Unknown exception caught!" << endl;
    return -1;
  }

  return 0;

}
