/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/TrackFitter.h"
#include "TrigFTKSim/FTKRoadFileInput.h"
#include "TrigFTKSim/FTKTrackFileOutput.h"
// output for firmware tests
//#include "TrigFTKSim/FTKFirmwareOutput.h"

#include "common_fcn.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// verbosity level
int trackfitter_verbosity(0);
// the track fitter implementation
TrackFitter TF;

int read_commands();

int main(int argc, char *argv[])
{
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  common_fcn_init(argc,argv);

  ftkset.setVerbosity(trackfitter_verbosity);

  // prepare the input and ouptut interfaces
  FTKRoadFileInput *road_input = new FTKRoadFileInput();
  FTKTrackFileOutput *track_output = new FTKTrackFileOutput();

  TF.setRoadInputModule(road_input);
  TF.setTrackOutputModule(track_output);

  // output for firmware tests
  //FTKFirmwareOutput *firmware_output = new FTKFirmwareOutput();
  //TF.setFirmwareOutputModule(firmware_output);

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
  
  int totevt = ftkset.getTotEvents();
  int ievt_step = totevt>500 ? totevt/100 : 1;
  // input module init
  cout << "Input initialization" << endl;
  TF.init();

  for (int ievt=0;ievt<totevt;++ievt) { // events processing loop
    if (ievt%ievt_step==0) {
      cout << "Processing evt # " << ievt << " / " << totevt << endl; 
      if (ievt%(ievt_step*10)==0)
	ftkset.usageStat();
    }

    int res = TF.nextEvent();
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
      TF.setNoConstantsErrorLevel(static_cast<ftk::message_level>(lvl));
    }
    else if (key=="PMAP_FILE") {
      string sval;
      sline >> sval;
      FTKPlaneMap *pmap = new FTKPlaneMap(sval.c_str()); 
      TF.setPlaneMap(pmap);
      TF.setNCoordsPlanes(pmap->getTotalDim(),pmap->getNPlanes());
    }
   else if (key=="CHI2_CUT") {
      float fval;
      sline >> fval;
      TF.setChi2Cut(fval);
    }
    else if (key=="CHI2_CUT5") {
      float fval;
      sline >> fval;
      TF.setChi2Cut_maj(fval);
    }
    else if (key=="CHI2_VETOMAJ") {
      float fval;
      sline >> fval;
      TF.setChi2Cut_vetomaj(fval);
    }
    else if (key=="CHI2_DOF") {
      float fval;
      sline >> fval;
      TF.setChi2DofCut(fval);
    }
    else if (key=="HITWARRIOR") {
      int ival;
      sline >> ival;
      TF.setHitWarrior(ival);
    }
    else if (key=="KEEP_REJECTED") {
      int ival;
      sline >> ival;
      TF.setKeepRejected(ival);
    }
    else if (key=="FIT_REMOVED") {
      int ival;
      sline >> ival;
      TF.setFitRemoved(ival);
    }
    else if (key=="HW_NDIFF") {
      int ival;
      sline >> ival;
      TF.setHWNDiff(ival);
    }
    else if (key=="HWSDEV_FILE") {
      string sval;
      sline >> sval;
      TF.loadHWConf(sval.c_str());
    }
    else if (key=="MAXNCOMB") {
      int ival;
      sline >> ival;
      TF.setMaxNcomb(ival);
    }
    else if (key=="MAXTRKOUT") {
      int ival;
      sline >> ival;
      TF.setMaxTrkout(ival);
    }
    else if (key=="NORECOVERY_NHITS") {
      int ival;
      sline >> ival;
      TF.setNorecoveryNhits(ival);
    }
    else if (key=="ONE_PER_ROAD") {
      int ival;
      sline >> ival;
      TF.setOnePerRoad(ival);
    }
    else if (key=="REQUIRE_FIRST") {
      bool ival;
      sline >> ival;
      TF.setRequireFirst(ival);
    }
    else if (key=="DO_MAJORITY") {
      unsigned int ival;
      sline >> ival;
      TF.setDoMajority(ival);
    }
    else if (key=="BANKS_SETUP") {
      int nreg, nsub;
      sline >> nreg >> nsub;
      TF.prepareBanks(nreg,nsub);
    }
    else if (key=="BANK") {
      // is a model or not
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


      // save the file position, used if the loop feature is used
      unsigned long int fpos = cin.tellg();

      for (int ir=regid;ir!=TF.getNumRegions();++ir) { // region loop
	cin.seekg(fpos);
	  
	for (int is=subid;is!=TF.getNumSubRegions();++is) { // sub-region loop
	  cin.seekg(fpos);

	  string bankpath;
	  replace_getline(cin,bankpath);


	  if (doRegLoop) {
	    replace_ids(bankpath,model_reg,ir);
	  }
	  if (doSubLoop) {
	    replace_ids(bankpath,model_sub,is);
	  }

	  if (doRegLoop||doSubLoop) {
	    cout << "Load bank [" << ir << "," << is << "]: " << bankpath << endl;
	  }

	  TF.setBank(ir,is,new FTKConstantBank(TF.getNCoords(),bankpath.c_str()));

	  if (!doSubLoop) break;
	} // end sub-region loop

	if (!doRegLoop) break;
      } // end region loop

    }
    else if (key=="INPUTFILE") {
      string sval;
      sline >> sval;
      TF.getRoadInputModule()->addFile(sval.c_str());
    }
    else if (key=="NEVENTS") {
       int ival;
       sline >> ival;
       ftkset.setTotEvents(ival);
    }
    else if (key=="SCTTRK_MODE") {
       int ival;
       sline >> ival;
       ftkset.setSCTtrkMode(ival);
    }
    else if (key=="MULT_OUT") {
      int ival;
      sline >> ival;
      if (ival) {
	TF.getTrackOutputModule()->setMultiOut(true);
      }
      else {
	TF.getTrackOutputModule()->setMultiOut(false);
      }
    }
    else if (key=="OUT_DIR") {
      string sval;
      sline >> sval;
      TF.getTrackOutputModule()->setOutDir(sval.c_str());
    }
    else if (key == "IBL_MODE") {
      int ival;
      sline >> ival;
      ftkset.setIBLMode(ival);
    }
    else if (key=="OUT_FILE") {
      string sval;
      sline >> sval;
      TF.getTrackOutputModule()->setFileName(sval.c_str());
    }
    // output for firmware tests
    //else if (key=="FWO_NSECTORS") {
    //  int ival;
    //  sline >> ival;
    //  TF.setNSectorsFWO(ival);
    //}
    else {
      cout << "*** unknown directive" << endl;
    }
  } // end loop over the configuration file
  
  return 0;
}
