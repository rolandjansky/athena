/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/FTKRawHit.h"
#include "common_fcn.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// path of the output file
string fakehits_path("output.dat");

// pattern bank under analysis
FTK_AMBank *bank(0x0);

// Plane map pointer, to be set during the initialization
FTKPlaneMap *pmap(0);

// Super-strip map, this object transform an hit in a SS index
FTKSSMap *ssmap(0);

// set if the SS use the HW-like mode
unsigned int HWModeSS(0);

// use hashmap internally in AM simulation (allows super-small SS widths)
bool force_am_hashmap(false);

// region map used in this session
FTKRegionMap *rmap(0);

int CUR_REGION(-1);

int read_commands();

/** main function */
int main(int argc, char *argv[]) {
  cout << "Patterns to hits conversion" << endl;
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  ftkset.setVerbosity(0);
  
  common_fcn_init(argc, argv);

  // preliminary parsing of the input file
  ifstream optionfile;
  streambuf *oldrdbuf = cin.rdbuf();
  if (argc > 1) {
    // input option in an external file
    optionfile.open(argv[1]);
    if (!optionfile) {
      cerr << "*** Error reading input file: " << argv[1] << endl;
      return -1;
    }
    // input option parsed from stdin
    cin.rdbuf(optionfile.rdbuf());
  }
  // call the parser
  if (read_commands() < 0) {
    cerr << "*** Error parsing the options" << endl;
    return -1;
  }
  cin.rdbuf(oldrdbuf);

  if (HWModeSS==0) {
    cerr << "Only HW-like SS can be currently used" << endl;
    return -1;
  }

  for (int ipatt=0;ipatt!=bank->getNPatterns();++ipatt) { // patterns loop
    FTKPattern curpatt = bank->getPattern(ipatt);

    for (int ilayer=0;ilayer!=curpatt.getNPlanes();++ilayer) { // layer loop
      // get the SSID
      int ssid = curpatt.getSSID(ilayer);      

      // prepare the hits
      FTKRawHit sshit;
      int type(-1);
      int atlas_layer(ilayer);
      int side(0);
      int phioff(0);
      int phimod(0);
      float localX(0);
      int etaoff(0);
      int etamod(0);
      float localY(0);
      int isEC(0);
      if (pmap->getPlane(ilayer,0).getNDimension()==2) {
	type = 1;

	// check if it is barrel or endcap SSID
	if (isEC==0) { // is a barrel SS
	  atlas_layer = pmap->getPlane(ilayer, 0).getLayer();
	  int section = pmap->getMap(type,isEC,atlas_layer).getSection();
	  ssmap->decodeSSTower(ssid,CUR_REGION,ilayer,section,phimod,etamod,localX,localY);

	}
	else { // it is an endcap SS
	  atlas_layer = pmap->getPlane(type,0).getLayer();
	}
      }
      else if (pmap->getPlane(ilayer,0).getNDimension()==1) {
	type = 0;
      }

      

      sshit.setHitType(type);
      sshit.setBarrelEC(isEC);
      sshit.setLayer(atlas_layer);
      sshit.setPhiModule(phimod);
      sshit.setEtaModule(etamod);
      sshit.setPhiSide(side);
      sshit.setEtaStrip(localX);
      sshit.setNStrips(localY);
      sshit.setEtaWidth(1.);
      sshit.setPhiWidth(1.);
      sshit.setX(0.); // dummy
      sshit.setY(0.); // dummy
      sshit.setZ(0.); // dummy
      sshit.setDeltaPhi(0.);
      sshit.setDeltaEta(0.);
      sshit.setIncludesGanged(false);
      //sshit.setTruth(const MultiTruth&); // truth for clusters
      
    } // end layer loop

  } // end patterns loop

  ftkset.usageStat();

  return 0;
}

/** this function parse the input commands and prepare a simple structure
    used by the main function */
int read_commands() {
  // ftk environemnt
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  
  const char prompt[] = "PARSE> ";
  
  string line;
  while (replace_getline(cin, line) != 0) {
    if (line[0] == '#' || line.size() == 0) {
      continue;
    }
    
    cout << prompt << line << endl;
    
    // extraxt the pair (key,value)
    istringstream sline(line);
    string key;
    sline >> key;
    if (key == "IGNORE_SIGSEGV") {
      // disable ROOT signal handler (it's buggy and may cause deadlocks after segfault)
      int ignore_sigsegv;
      sline >> ignore_sigsegv;
      if (ignore_sigsegv)
	gSystem->IgnoreSignal(kSigSegmentationViolation, true);
    } else if (key == "VERBOSITY") {
      int ival;
      sline >> ival;
      ftkset.setVerbosity(ival);
    } else if (key == "HWMODEID") {
      sline >> HWModeSS;
      cout << prompt << key << " = " << HWModeSS << endl;
    } else if (key == "SS_FILE") {
      string sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      ssmap = new FTKSSMap(rmap, sval.c_str(), force_am_hashmap == false);
    } else if (key == "IBL_MODE") {
      int ival;
      sline >> ival;
      ftkset.setIBLMode(ival);
    } else if (key == "CUR_REGION") {
      int ival;
      sline >> ival;
      CUR_REGION = ival;
    } else if (key == "FORCE_AM_HASHMAP") {
      bool ival;
      sline >> ival;
      force_am_hashmap = ival;
    } else if (key == "BANK") {
      int ibank, maxpatt;
      string bankpath;
      /* retrieve the number of the bank the tha max number 
	 of patterns to read (-1 means all patterns) */
      sline >> ibank >> maxpatt;
      // the next line is the bank path
      replace_getline(cin, bankpath);
      bank = new FTK_AMBank(ibank,0);

      bank->setHWModeSS(HWModeSS);
      
      
      if (bankpath.substr(bankpath.size() - 4) == string("root")) {
	// use the ROOT routine
	if (bank->readROOTBank(bankpath.c_str(), maxpatt) < 0) {
	  // error reading the file
	  delete bank;
	  return -1;
	}
      } else {
	// if the previous check failed try with ASCII file
	if (bank->readASCIIBank(bankpath.c_str(), maxpatt) < 0) {
	  // error reading the file
	  delete bank;
	  return -1;
	}
      }
      ftkset.usageStat();
    } else if (key == "PMAP_FILE") {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap = new FTKPlaneMap(sval.c_str());
      if (!(*pmap)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	return -1;
      }
    } else if (key == "RMAP_FILE") {
      // set rmap file
      string sval;
      sline >> sval;
      rmap = new FTKRegionMap(pmap, sval.c_str());
      if (!(*rmap)) {
	cerr << "*** error using map from: " << sval << endl;
	return -1;
      }
    } else if (key == "OUT_FILE") {
      sline >> fakehits_path;
    } else {
      cout << prompt << "\"" << key << "\" is not a valid key" << endl;
      continue;
    }
  }
    
  return 0;
}
