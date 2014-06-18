/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/TrackFitter711.h"
#include "TrigFTKSim/FTKRoadFileInput.h"
#include "TrigFTKSim/FTKTrackFileOutput.h"
#include "common_fcn.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// Super-strip map, this object transform an hit in a SS index
FTKSSMap *ssmapI(0);
FTKSSMap *ssmapC(0);
// region map used in this session
FTKRegionMap *rmapI(0);
FTKRegionMap *rmapC(0);
// Plane map pointer, to be set during the initialization
FTKPlaneMap *pmapI(0); // incomplete set
FTKPlaneMap *pmapC(0); // complete set

// verbosity level
int trackfitter_verbosity(0);
// the track fitter implementation
TrackFitter711 TF711;

int read_commands();

int main(int argc, char *argv[])
{
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  common_fcn_init(argc,argv);

  // prepare the input and ouptut interfaces
  FTKRoadFileInput *road_input = new FTKRoadFileInput();
  FTKTrackFileOutput *track_output = new FTKTrackFileOutput();
  
  TF711.setRoadInputModule(road_input);
  TF711.setTrackOutputModule(track_output);

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
    return -1;
  }
  cin.rdbuf(oldrdbuf);

  ftkset.setVerbosity(trackfitter_verbosity);

  int totevt = ftkset.getTotEvents();
  int ievt_step = totevt>500 ? totevt/100 : 1;
  // input module init
  cout << "Input initialization" << endl;
  TF711.init();

  for (int ievt=0;ievt<totevt;++ievt) { // events processing loop
    if (ievt%ievt_step==0) {
      cout << "Processing evt # " << ievt << " / " << totevt << endl; 
      if (ievt%(ievt_step*10)==0)
	ftkset.usageStat();
    }

    int res = TF711.nextEvent();
    if (res<0) { // error
      cerr << "*** error reading event # " << ievt << endl;
      break;
    }
  } // events procesing loop

  ftkset.usageStat();

  // destroy the I/O interfaces
  delete road_input;
  delete track_output;

  return 0;
}

int read_commands() {
   FTKSetup &ftkset = FTKSetup::getFTKSetup();

  string line;
  while (replace_getline(cin,line)) { // loop over the configuration file
    istringstream sline(line);
    cout << "PARSE> " << line << endl;
    // skip empty lines and comments
    if (!line.size()) continue;
    else if (line[0]=='#') continue;

    string key;
    sline >> key;

    if (key=="VERBOSITY") {
      sline >> trackfitter_verbosity;
    }
    else if (key=="NOCONSTANTS_ERRLEVEL") {
      int lvl;
      sline >> lvl;
      TF711.setNoConstantsErrorLevel(static_cast<ftk::message_level>(lvl));
    }
    else if (key == "MAX_MISSING_EXTRAPLANES") {
       int ival;
       sline >> ival;
       TF711.setMaxMissingExtraPlanes(ival);
    }
    else if (key=="EXTRAFITS") {
      int lvl;
      sline >> lvl;
      TF711.setExtraFits(lvl);
    }
    else if (key=="NUMBER_NEIGHBOURS") {
      int val;
      sline >> val;
      TF711.setNNeighbours(val);
    }
    else if (key=="PMAP_FILE_INCOMPLETE") {
      string sval;
      sline >> sval;
      pmapI = new FTKPlaneMap(sval.c_str());
      TF711.setPlaneMapIncomplete(pmapI);
    }
    else if (key=="PMAP_FILE_COMPLETE") {
      string sval;
      sline >> sval;
      pmapC = new FTKPlaneMap(sval.c_str());
      TF711.setPlaneMap(pmapC);
    }
    else if (key=="RMAP_FILE") {
      // set rmap file
      string sval;
      sline >> sval;
      rmapC = new FTKRegionMap(pmapC,sval.c_str());
      if (!(*rmapC)) {
	cerr << "*** error using map from: " << sval << endl;
	return -1;
      }
      rmapI = new FTKRegionMap(pmapI,sval.c_str());
    }
    else if (key == "SS_FILE_INCOMPLETE") { // 7 layer fit for 7+11 architecture
       string sval;
       sline >> sval;
       cout << key << " = " << sval << endl;
       ssmapI = new FTKSSMap(rmapI,sval.c_str(),false);
       TF711.setSSMapIncomplete(ssmapI);
    }
    else if (key == "SS_FILE_COMPLETE") { // 11 layer fit for 7+11 architecture
       string sval;
       sline >> sval;
       cout << key << " = " << sval << endl;
       ssmapC = new FTKSSMap(rmapC,sval.c_str(),false);
       TF711.setSSMapComplete(ssmapC);
    }
   else if (key=="CHI2_CUT") {
      float fval;
      sline >> fval;
      TF711.setChi2Cut(fval);
    }
    else if (key=="CHI2_CUT5") {
      float fval;
      sline >> fval;
      TF711.setChi2Cut_maj(fval);
    }
    else if (key=="CHI2_VETOMAJ") {
      float fval;
      sline >> fval;
      TF711.setChi2Cut_vetomaj(fval);
    }
    else if (key=="CHI2_DOF") {
      float fval;
      sline >> fval;
      TF711.setChi2DofCut(fval);
    }
    else if (key=="HITWARRIOR") {
      int ival;
      sline >> ival;
      TF711.setHitWarrior(ival);
    }
    else if (key=="KEEP_REJECTED") {
      int ival;
      sline >> ival;
      TF711.setKeepRejected(ival);
    }
    else if (key=="FIT_REMOVED") {
      int ival;
      sline >> ival;
      TF711.setFitRemoved(ival);
    }
    else if (key=="HW_NDIFF") {
      int ival;
      sline >> ival;
      TF711.setHWNDiff(ival);
    }
    else if (key=="HWSDEV_FILE") {
      string sval;
      sline >> sval;
      // set the number of the coordinates
      TF711.setNCoordsPlanes(pmapI->getTotalDim(),pmapI->getNPlanes(),
			     pmapC->getTotalDim(),pmapC->getNPlanes());
      TF711.loadHWConf(sval.c_str());
    }
    else if (key=="MAXNCOMB") {
      int ival;
      sline >> ival;
      TF711.setMaxNcomb(ival);
    }
    else if (key=="MAXTRKOUT") {
      int ival;
      sline >> ival;
      TF711.setMaxTrkout(ival);
    }
    else if (key=="NORECOVERY_NHITS") {
      int ival;
      sline >> ival;
      TF711.setNorecoveryNhits(ival);
    }
    else if (key=="ONE_PER_ROAD") {
      int ival;
      sline >> ival;
      TF711.setOnePerRoad(ival);
    }
    else if (key=="REQUIRE_FIRST") {
      bool ival;
      sline >> ival;
      TF711.setRequireFirst(ival);
    }
    else if (key=="DO_MAJORITY") {
      unsigned int ival;
      sline >> ival;
      TF711.setDoMajority(ival);
    }
    else if (key=="SAVE_INCOMPLETE") {
      int ival;
      sline >> ival;
      TF711.setSaveIncompleteTracks(ival);
    }
    else if (key=="MAKE_DIAGNOSTIC_PLOT") {
      int ival;
      sline >> ival;
      TF711.setDiagnosticMode(ival);
    }
    else if (key=="TRACKFITTER_MODE") {
      int ival;
      sline >> ival;
      if (ival==0) {
	// default mode: synched sectors
	TF711.setResolutionMode(false);
	TF711.setUseSectorDB(false);
      }
      else if (ival==1) {
	// use the connection DB file
	TF711.setResolutionMode(false);
	TF711.setUseSectorDB(true);
      }
      else if (ival==2) {
	// run in resolution mode
	TF711.setResolutionMode(true);
	//TF711.setUseSectorDB(false);
	TF711.setUseSectorDB(true);
      }
      else if (ival==3) {
	// use the connection DB file
	TF711.setResolutionMode(false);
	TF711.setUseSectorDB(true);
	TF711.setSuperExtrapolateMode(true);
      }
    }
    else if (key=="CHECK_TRANSITION_BY") {
      int ival;
      sline >> ival;
      TF711.setCheckTRby(ival);
    }
    else if (key=="ALLOW_EXTRA_MISS_IN_SUPEREXP") {
      int ival;
      sline >> ival;
      TF711.setAllowExtraMissInSuperExp(ival);
    }
    else if (key=="MAX_TRANSITION_ETA") {
      float fval;
      sline >> fval;
      TF711.setTransitionRegionMaxEta(fval);
    }
    else if (key=="MIN_TRANSITION_ETA") {
      float fval;
      sline >> fval;
      TF711.setTransitionRegionMinEta(fval);
    }
    else if (key=="NCONN") {
      int ival;
      sline >> ival;
      // Use up to N connection
      TF711.setUseNConn(ival);
    }
    else if (key=="MULTI_CONN_MODE") {
      int ival;
      sline >> ival;
      if (ival==0) {
	// single connection baseDS
	TF711.setUseMultipleConn(false);
      }
      else if (ival==1) {
	// multiple connection baseDS
	TF711.setUseMultipleConn(true);
      }
    }
    else if (key=="USEGUESSING") {
      int ival;
      sline >> ival;
      if (ival==1) { // default
	TF711.setUseGuessing(true);
      }
      else if (ival==0) {
	// use geometrical extrapolation
	TF711.setUseGuessing(false);
      }
      else {
	cerr << "!!! Guessing algorithm '" << ival << "' not valid, use majority" << endl;
      }
    }
    else if (key=="BANKS_SETUP") {
      int nreg, nsub;
      sline >> nreg >> nsub;
      TF711.prepareBanks(nreg,nsub);
    }
    else if (key=="BANK") {
      // is a model or not?
      bool doRegLoop(false);
      string model_reg;
      int regid(-1);
      bool doSubLoop(false);
      string model_sub;
      int subid(-1);
      sline >> model_reg >> model_sub;

      // if a key start with $ means is variable, this will
      // serached in the following strings to be modified
      // with the region or sub-region ids
      if (model_reg[0] == '$') {
	cout << "Enabling region loop with model: " << model_reg << endl;
	doRegLoop = true;
	regid = 0;
      }
      else {
	regid = atoi(model_reg.c_str());	
      }

      if (model_sub[0] == '$') {
	cout << "Enabling sub-region loop with model: " << model_sub << endl;
	doSubLoop = true;
	subid = 0;
      }
      else {
	subid = atoi(model_sub.c_str());
      }

      //doSubLoop = true;


      // save the file position, used if the loop feature is used
      unsigned long int fpos = cin.tellg();

      for (int ir=regid;ir!=TF711.getNumRegions();++ir) { // region loop
	cin.seekg(fpos);
	  
	for (int is=subid;is!=TF711.getNumSubRegions();++is) { // sub-region loop
	//for (int is=0;is!=TF711.getNumSubRegions();++is) { // sub-region loop
	  cin.seekg(fpos);

	  // path of the incomplete bank
	  string bankpathI;
	  // path of the complete bank
	  string bankpathC;
	  // path with the file containing the relations between incomplete and complete sectors
	  string connpath;
	  replace_getline(cin,bankpathI);
	  replace_getline(cin,bankpathC);
	  replace_getline(cin,connpath);


	  if (doRegLoop) {
	    replace_ids(bankpathI,model_reg,ir);
	    replace_ids(bankpathC,model_reg,ir);
	    replace_ids(connpath,model_reg,ir);
	  }
	  if (doSubLoop) {
            string tag = "sub";
	    replace_ids_with_tag(bankpathI,model_sub,tag,is);
	    replace_ids_with_tag(bankpathC,model_sub,tag,is);
	    replace_ids_with_tag(connpath ,model_sub,tag,is);
	  }

	  cout << "Load incomplete bank [" << ir << "," << is << "]: " << bankpathI << endl;
	  cout << "Load complete bank [" << ir << "," << is << "]: " << bankpathC << endl;
	  cout << "Connection file bank [" << ir << "," << is << "]: " << connpath << endl;

	  FTKConstantBank *tmpbankC = 0x0;
	  FTKConstantBank *tmpbankI = 0x0;
	  FTKSector711DB *tmpconn = 0x0;
	  if (bankpathC!="null")
	    tmpbankC = new FTKConstantBank(TF711.getNCoordsComplete(),bankpathC.c_str());
	  if (bankpathI!="null")
	    tmpbankI = new FTKConstantBank(TF711.getNCoordsIncomplete(),bankpathI.c_str());
	  if (connpath!="null")
	    tmpconn = new FTKSector711DB(tmpbankI->getNSectors(),pmapC->getNPlanes()-pmapI->getNPlanes(),connpath.c_str());
	  TF711.setBank(ir,is,tmpbankC,tmpbankI,tmpconn);

	  if (!doSubLoop) break;
	} // end sub-region loop

	if (!doRegLoop) break;
      } // end region loop

    }
    else if (key=="INPUTFILE") {
      string sval;
      sline >> sval;
      TF711.getRoadInputModule()->addFile(sval.c_str());
    }
    else if (key=="NEVENTS") {
       int ival;
       sline >> ival;
       ftkset.setTotEvents(ival);
    }
    else if (key=="MULT_OUT") {
      int ival;
      sline >> ival;
      if (ival) {
	TF711.getTrackOutputModule()->setMultiOut(true);
      }
      else {
	TF711.getTrackOutputModule()->setMultiOut(false);
      }
    }
    else if (key=="OUT_DIR") {
      string sval;
      sline >> sval;
      TF711.getTrackOutputModule()->setOutDir(sval.c_str());
    }
    else if (key == "IBL_MODE") {
      int ival;
      sline >> ival;
      ftkset.setIBLMode(ival);
    }
    else if (key=="OUT_FILE") {
      string sval;
      sline >> sval;
      TF711.getTrackOutputModule()->setFileName(sval.c_str());
    }
    else {
      cout << "*** unknown directive" << endl;
    }
  } // end loop over the configuration file

  return 0;
}
