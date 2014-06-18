/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  General utility that performs TrigFTKLib functionality.
  Currently it can filter single muons, make TRK_LOOKUP, make sectors
  It is trivial to add other things, such as making patterns
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKCacheLookup.h"
#include "TrigFTKSim/FTK_SingleTrackInput.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <TString.h>
using namespace std;

bool sectorMode(false);
bool pattgenMode(false); // unused yet!
bool patternMode(false); // unused yet!
bool trackMode(false);

// Main map files to be used for pattern / sector generation
FTK_SingleTrackInput *dinput(0);
FTKPlaneMap *pmap(0);
FTKRegionMap *rmap(0);
FTKSSMap *ssmap(0);

// Auxillary map files to be used for region lookup
bool reglookup11(false); // perform region lookup on 11L events
FTK_SingleTrackInput *dinput11(0);
FTKPlaneMap *pmap11(0);
FTKRegionMap *rmap11(0);

int NBANKS(-1);
// 
std::vector<string> bankpaths;
std::vector<string> secpaths;
std::vector<string> gconpaths;
TString outfile;
int NSUBS(16);
int NCOORDS(14);
int CUR_REGION(-1);
double CHI2_CUT(14);

int read_commands();

/** main function */
int main(int argc, char *argv[]) {
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  ftkset.setVerbosity(0);

  ofstream trackout;
  FTKTrackLookup tracklookup;

  // preliminary parsing of the input file
  ifstream optionfile;
  streambuf *oldrdbuf = cin.rdbuf();
  if (argc>1) { 
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
  if (read_commands()<0) {
    cerr << "*** Error parsing the options" << endl;
  }
  cin.rdbuf(oldrdbuf);

  const bool rootFormat = outfile.EndsWith("root");
  if(trackMode) {
    cout << "Dumping tracks into " << outfile << endl;
    if(rootFormat) {
      tracklookup.open_for_write(outfile.Data()); 
    }
    else {
      trackout.open(outfile.Data());
      assert(trackout.is_open());
    }
  }

  int totevt = ftkset.getTotEvents();
  int ievt_step = totevt>500 ? totevt/100 : 1;
  // input module init
  cout << "I/O initialization" << endl;

  ifstream fin("/home/antonk/single_muons.dat");
  std::string line;

  // in sector mode, initialize structures to store sectors in each region
  if(sectorMode) {
    for (int reg=0; reg<rmap->getNumRegions(); reg++) {
      dinput->reserveSectors(reg,true);
    }
  }

  // in track mode, load sector and gcon banks
  if(trackMode) {
    for (int sub=0; sub<NSUBS; sub++) {
      dinput->loadSectors(sub,secpaths[sub].c_str());
      dinput->loadConstants(sub,gconpaths[sub].c_str());
    }
  }

  for (int ievt=0;ievt<totevt;++ievt) { // events processing loop
    if (ievt%ievt_step==0) {
      cout << "Processing evt # " << ievt << " / " << totevt << endl; 
      if (ievt%(ievt_step*10)==0)
	ftkset.usageStat();
    }
    // move to the next event
    int res = dinput->readData();
    if (res<0) {
      cerr << "*** error reading event # " << ievt << endl;
      break;
    }
    if(dinput->getNTruthTracks()!=1) {
      continue;
    }
    // filter the hits
    dinput->processEvent(false);
    if(dinput->filterHits()==-1) {
      continue;
    }

    // determine the region
    int region(-1);
    if(!reglookup11) // just find the region using rmap
      region = dinput->Region();
    else {           // OR: use 11L configuration to determine the region
      dinput11->bootstrapEvent(dinput);
      dinput11->processEvent();
      if(dinput11->filterHits()==-1) {
	continue;
      }
      region = dinput11->Region();
    }
    if(region==-1) continue;

    // find the sector
    if(sectorMode) {
      dinput->getSector();
      // Debug:
      // cout << "Event number " << dinput->globEventNumber() << endl;
      // cout << "Region " << region << " " << dinput->Sector() << endl;
      dinput->saveSector(region);
    }

    // fit the track
    if(trackMode) {
      if(region!=CUR_REGION) { // only in current region
	continue;
      }
      if(dinput->findSector()==-1) { // need to find the sector
	continue;
      }
      dinput->performFit();
      double chi2 = dinput->Track().getChi2();
      if(chi2 < CHI2_CUT)
	if(rootFormat)
	  dinput->DumpTrack(tracklookup);
	else
	  dinput->DumpTrack(trackout);
      else 
	std::cout << dinput->globEventNumber() << ": bad chi2 = " << chi2 << " (cut = " << CHI2_CUT << ")" << endl;
    }

  } // events procesing loop

  if(sectorMode) {
    // in sector mode, save the sectors in all regions
    for(int reg=0; reg<NBANKS; reg++) {
      dinput->getSectorBank(reg)->SaveAs(bankpaths[reg].c_str());
    }
  }

  if(patternMode || pattgenMode) {
    // in pattern mode, save the patterns in all subregions
    for(int sub=0; sub<NSUBS; sub++) {
      dinput->getPatternBank(sub)->SaveAs(bankpaths[sub].c_str());
    }
  }

  ftkset.usageStat();

  if(trackMode) {
    if(rootFormat)
      tracklookup.done();
    else
      trackout.close();
  }

  return 0;
}


/** this function parse the input commands and prepare a simple structure
    used by the main function */
int read_commands()
{
  // ftk environemnt
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  const char prompt[] = "PARSE> ";

  string line;
  while(getline(cin,line)!=0) {    
    if (line[0]=='#' || line.size()==0) {
      continue;
    }

    cout << prompt << line << endl;

    // extraxt the pair (key,value)
    istringstream sline(line);
    string key;
    sline >> key;
    if (key=="events#") { // set the number of events to process
      int ival;
      sline >> ival;
      cout << prompt << ival << " events will be processed" << endl;
      ftkset.setTotEvents(ival);
    }
    else if (key=="VERBOSITY") {
      int ival;
      sline >> ival;
      ftkset.setVerbosity(ival);
    }
    else if (key=="SECTOR_MODE") {
      sline >> sectorMode;
    }
    else if (key=="PATTGEN_MODE") {
      sline >> pattgenMode;
    }
    else if (key=="PATTERN_MODE") {
      sline >> patternMode;
    }
    else if (key=="TRACK_MODE") {
      sline >> trackMode;
    }
    else if (key=="BARREL_ONLY") {
      int ival;
      sline >> ival;
      cout << prompt << "BARREL_ONLY = " << ival << endl;
      ftkset.setBarrelOnly(ival);
    }
    else if (key == "SS_FILE") {
       string sval;
       sline >> sval;
       cout << prompt << key << " = " << sval << endl;
       ssmap = new FTKSSMap(rmap,sval.c_str(),false);
    }
    else if (key == "SS_OFFSET") {
      double sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      if (!ssmap) {
	cerr << "*** error: please set SS_FILE before SS_OFFSET" << sval << endl;
	return -1;
      }
      ssmap->setOffsetFraction(sval);
    }
    else if (key=="CUR_REGION") {
      int ival;
      sline >> ival;
      CUR_REGION=ival;
     }
    else if (key=="NSUBS") {
      sline >> NSUBS;
    }
    else if (key=="NCOORDS") {
      sline >> NCOORDS;
    }
    else if (key=="CHI2_CUT") {
      sline >> CHI2_CUT;
    }
    else if (key=="NBANKS") {
      sline >> NBANKS;
      // in sector mode, NBANKS=nregions
      // and we read the names of the sector files for each region
      if(sectorMode) {
	for(int i=0; i<NBANKS; i++) {
	  std::string bankpath;
	  getline(cin,bankpath);
	  bankpaths.push_back(bankpath);
	}
      }
      // in track fitting mode, NBANKS=nsubregions,
      // and we read pairs of "sector \n constant" file for each subregion
      if(trackMode) {
	for(int i=0; i<NBANKS*2; i++) {
	  std::string secpath;
	  std::string gconpath;
	  getline(cin,secpath);
	  getline(cin,gconpath);
	  secpaths.push_back(secpath);
	  gconpaths.push_back(gconpath);
	}
      }
    }
    else if (key=="RAW_MODE") {
      int ival;
      sline >> ival;
      ftkset.setRawMode(ival);
      switch (ival) {
      case 1:
	if (!pmap || !rmap || !ssmap) {
	  cout << "*** RAW data input module needs a valid PMap, RegionMap, and SSMap" << endl;
	  cout << "*** Call PMAP_FILE, RMAP_FILE, and SS_FILE before RAW_MODE" << endl;
	  return -1;
	}
	// Main map files to be used for pattern / sector generation
	if (true) {
	  dinput = new FTK_SingleTrackInput(pmap,rmap,ssmap);
	  dinput->setNSubRegions(NSUBS);
	  dinput->setNCoords(NCOORDS);
	  dinput->singleMuonMode();
	}
	// Auxillary map files to be used for region lookup
	if (pmap11 && rmap11 && reglookup11) {
	  dinput11 = new FTK_SingleTrackInput(pmap11,rmap11);
	  dinput11->setNSubRegions(NSUBS);
	  dinput11->setNCoords(NCOORDS);
	  dinput11->singleMuonMode();
	}
	break;
      default:
	cout << "*** RAW_MODE=" << ival << " is not supported" << endl;
	return -1;
      }
    }
    else if (key=="PMAP_FILE") {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap = new FTKPlaneMap(sval.c_str());
      if (!(*pmap)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	return -1;
      }
    }
    else if (key=="RMAP_FILE") {
      // set rmap file
      string sval;
      sline >> sval;
      if (!pmap) {
	cout << "*** RegionMap needs a valid PMap" << endl;
	cout << "*** Call PMAP_FILE before RMAP_FILE" << endl;
	return -1;
      }
      rmap = new FTKRegionMap(pmap,sval.c_str());
      if (!(*rmap)) {
	cerr << "*** error using map from: " << sval << endl;
	return -1;
      }
    }
    else if (key=="REGIONS_11L") {
      sline >> reglookup11;
      if(reglookup11) {
	cout << "Region will be determined from 11L configuration" << std::endl;
      }
    }
    else if (key=="PMAP_FILE11" && reglookup11) {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap11 = new FTKPlaneMap(sval.c_str());
      if (!(*pmap11)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	return -1;
      }
    }
    else if (key=="RMAP_FILE11" && reglookup11) {
      // set rmap file
      string sval;
      sline >> sval;
      if (!pmap11) {
	cout << "*** RegionMap needs a valid PMap" << endl;
	cout << "*** Call PMAP_FILE11 before RMAP_FILE11" << endl;
	return -1;
      }
      rmap11 = new FTKRegionMap(pmap11,sval.c_str());
      if (!(*rmap11)) {
	cerr << "*** error using map from: " << sval << endl;
	return -1;
      }
    }
    else if (key=="DIAG_CLUSTERING") {
      int ival;
      sline >> ival;
      dinput->setDiagClustering(ival);
      dinput->initClustering();
    }
    else if (key == "SCT_CLUSTERING") {
      int ival;
      sline >> ival;
      dinput->setSctClustering(ival);
    }
    else if (key=="DUPLICATE_GANGED") {
      int ival;
      sline >> ival;
      dinput->setDuplicateGanged(ival);
      dinput->initClustering();
    }
    else if (key=="GANGED_PATTERN_RECOGNITION") {
      int ival;
      sline >> ival;
      dinput->setGangedPatternRecognition(ival);
      dinput->initClustering();
    }
    else if (key=="FTKDAT_LIST") {
      // add this list of files as input
      string sval;
      sline >> sval;
      int res = dinput->addFilesList(sval.c_str());
      cout << "Added: " << res << " files" << endl;
    }
    else if (key=="OUT_FILE") {
      std::string tmpstring;
      sline >> tmpstring;
      outfile = tmpstring;
    }
    else {
      cout << prompt << "\"" << key << "\" is not a valid key" << endl;
      continue;
    }
  }

  return 0;
}
