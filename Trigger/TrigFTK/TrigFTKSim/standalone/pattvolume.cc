/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/RoadFinder.h"
#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"
#include "TrigFTKSim/FTK_RawInput.h"
#include "TrigFTKSim/FTKRoadFileOutput.h"
#include "TrigFTKSim/FTKSectorMap.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "common_fcn.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// instance of the RoadFinder object
RoadFinder rfobj;

// ss offset (set through config file)
double ss_offset_fraction = 0;

// set the use of the TSP DB banks
int useTSPBank(0);
// if the TSP bank is used it is possible to ask a specific level
int BankLevel(1);
// set if the TSP simulation is performed, 0 not used
int doTSPSim(0);
// minimum TSP coverage, if >1 the way the AM bank is built change
int minTSPCoverage(1);
// store the variable if the TSP bank cache has to be saved
int doMakeCache(0);
// path of the output file
string CachePath("bankcache.root");

int SaveAllRoads(0);

// Plane map pointer, to be set during the initialization
FTKPlaneMap *pmap(0);
FTKPlaneMap *pmap_unused(0);
// Super-strip map, this object transform an hit in a SS index
FTKSSMap *ssmap(0);
FTKSSMap *ssmap_unused(0);
FTKSSMap *ssmap_tsp(0);
// require presence of first layer (pixel B-layer)
bool require_first(false);
// use hashmap internally in AM simulation (allows super-small SS widths)
bool force_am_hashmap(false);
// region map used in this session
FTKRegionMap *rmap(0);
FTKRegionMap *rmap_unused(0);
int CUR_REGION(-1);
// lists containing output of 8L run
std::string scttrk_tracklist;
std::string scttrk_roadlist;

std::string m_badmap_path; 
std::string m_badmap_path2; 

// 4L,8L -> 11L lookup map
FTKSectorMap *scttrk_sectormap(0);

int read_commands();

double chi2f (double *k, size_t dim, void *params);
// parameters for chi2f
int nconstr, ncoords;
float *kaverage; // kaverage[nconstr]; 
float **kernel;   // kernel[nconstr][ncoords];

/** main function */
int main(int argc, char *argv[]) {
  cout << "Pattern Volume" << endl;
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  ftkset.setVerbosity(0);
  
  common_fcn_init(argc, argv);

  // Prepare the output module
  FTKRoadFileOutput *road_output = new FTKRoadFileOutput();
  rfobj.setRoadOutputModule(road_output);

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

  cout << "Init ConstantBank" << endl;
  //ssmap and pmap are global and are initialized in read_commands()
  FTKConstantBank *cnstBank = new FTKConstantBank(pmap->getTotalDim(), "/home/cascella/FTK/data/gcon/corrgen_raw_7L_16M_reg0_sub0.gcon.bz2");
  
  cout << "Get PattBank" << endl;
  //I read the bank id from the conf file but at some point we are going to have read_commands save this somewhere
  FTK_AMBank *pattBank = rfobj.getAMBank(0);

  cout << "Get Pattern 0" << endl;
  // void FTKSSMap::decodeSS	(int SSid,const int &plane, const int &section, int &phioff, int &phimod, int &localX, int &etaoff )	
  FTKPattern patt = pattBank->getPattern(0);

  cout << " Pattern 0 : " << endl;
  patt.Print();
  cout << endl;
  cout << " sector coverage " 
       << pattBank->getSectorCoverage(patt.getSectorID())
       << " sectortotal coverage " 
       << pattBank->getTotalCoverage()
       << endl;

  double x[20];
  int nplanes = patt.getNPlanes();
  for (int iplane=0;iplane!=nplanes;++iplane) { // loop over the planes
    int SSid, section, phioff, phimod, localX, etaoff, phiss;
    SSid = patt.getSSID(iplane);
    section = 0;//patt.getSectorID();
    cout << "SSid " << SSid 
	 << " plane " << iplane
	 << " section " << section << endl;

    ssmap->decodeSS(SSid, iplane, section, phioff, phimod, localX, etaoff); 
    cout << " phioff " << phioff 
	 << " phimod " << phimod 
	 << " localX " << localX 
	 << " etaoff " << etaoff << endl;
    FTKHit hit;
    hit.setPlane(iplane);
    hit.setSector(section);
    phiss = ssmap->getSSPhiWidth(hit); 
    cout << " phiSS " << phiss << endl;
    x[iplane] = localX;
  }
  
  //setting up chi2f parameters 
  nconstr = cnstBank->getNConstr();
  ncoords = cnstBank->getNCoords();
  kaverage = cnstBank->getKaverage(patt.getSectorID());
  kernel = cnstBank->getKernel(patt.getSectorID());

  cout.precision(2);
  cout << " kaverage["<<nconstr<<"]: ";
  for(int i=0;i<nconstr;++i){
    cout << kaverage[i] << "\t";
  }
  cout << endl;

  cout << " kernel["<<nconstr<<"]["<<ncoords<<"]: " << endl;
  for(int i=0;i<nconstr;++i){
    for(int j=0;j<ncoords;++j) {
      cout << kernel[i][j] << "\t";
    }
    cout << endl;
  }


  // TrackFitter.cxx
//        for (int ip=0;ip<nplanes;++ip) {
// 	 // retrieve the coordinate in the final array
// 	 int ix = m_pmap->getDim(ip,0);
// 	 int iy = m_pmap->getDim(ip,1);
// 	 // decide if a 1D or 2D hit
// 	 int ndim = iy==-1 ?  1 : 2;
// 	 if (position[ip]!=endlist[ip]) {
// 	   if (ndim==1) {
// 	     newtrk.setCoord(ix,(*position[ip])[0]);
// 	   }
// 	   else {
// 	     newtrk.setCoord(ix,(*position[ip])[0]);
// 	     newtrk.setCoord(iy,(*position[ip])[1]);
// 	   }
// 	 }
//        }
//        newtrk.setTrackID(m_comb_id);
//        newtrk.setHWRejected(HWbase);
//        newtrk.setHWTrackID(-1);




  cout << "chi^2(localX???) = " 
       << chi2f(x,10,0) 
       << endl;


  //  int totevt = ftkset.getTotEvents();
  //  int ievt_step = totevt > 500 ? totevt / 100 : 1;
  // input module init
  //  cout << "I/O initialization" << endl;
  // instanciate the output module
  //  rfobj.init();
 //  for (int ievt = 0; ievt < totevt; ++ievt) { // events processing loop
 //    if (ievt % ievt_step == 0) {
 //      cout << "Processing evt # " << ievt << " / " << totevt << endl;
 //      if (ievt % (ievt_step * 10) == 0)
 //	ftkset.usageStat();
 //    }
 //    
 //    int res = rfobj.nextEvent();
 //    if (res < 0) { // error
 //      cerr << "*** error reading event # " << ievt << endl;
 //      break;
 //    }
 //  } // events procesing loop
 //
 //  ftkset.usageStat();
 //
  // destroy the I/O modules
  //  delete road_output;
  //  delete rfobj.getDataInputModule();
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
    } else if (key == "events#") {
      int ival;
      sline >> ival;
      cout << prompt << ival << " events will be processed" << endl;
      ftkset.setTotEvents(ival);
    } else if (key == "VERBOSITY") {
      int ival;
      sline >> ival;
      ftkset.setVerbosity(ival);
    } else if (key == "BARREL_ONLY") {
      int ival;
      sline >> ival;
      cout << prompt << "BARREL_ONLY = " << ival << endl;
      ftkset.setBarrelOnly(ival);
    } else if (key == "ENABLE_FTKSIM") {
      int ival;
      sline >> ival;
      cout << prompt << key << " = " << ival << endl;
      ftkset.setEnableFTKSim(ival);
    } else if (key == "SS_FILE") {
      string sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      ssmap = new FTKSSMap(rmap, sval.c_str(), force_am_hashmap == false);
    } else if (key == "SS_FILE_UNUSED") {
      string sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      rmap_unused = new FTKRegionMap(pmap_unused, rmap->getPath());
      // the map on unused layer skip the boundary check by default
      ssmap_unused = new FTKSSMap(rmap_unused, sval.c_str(), false);
    } else if (key == "SS_FILE_TSP") {
      string sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      // the map on unused layer skip the boundary check by default
      ssmap_tsp = new FTKSSMap(rmap, sval.c_str(), false);
    } else if (key == "SS_OFFSET") {
      double sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      ss_offset_fraction = sval;
    } else if (key == "MAX_MISSING_PLANES") {
      int ival;
      sline >> ival;
      cout << prompt << key << " = " << ival << endl;
      ftkset.setMaxMissingPlanes(ival);
    } else if (key == "ROADWARRIOR") {
      int ival;
      sline >> ival;
      cout << prompt << key << " = " << ival << endl;
      ftkset.setRoadWarrior(ival);
    } else if (key == "KEEP_REMOVED") {
      int ival;
      sline >> ival;
      ftkset.setKeepRemoved(ival);
    } else if (key == "INPUTFILE") {
      // add a single file to the list
      string sval;
      sline >> sval;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      if (dinput->addFile(sval.c_str()) != -1) {
	cout << "File: " << sval << " added" << endl;
      }
    } else if (key == "FTKDAT_LIST") {
      // add this list of files as input
      string sval;
      sline >> sval;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      int res = dinput->addFilesList(sval.c_str());
      cout << "Added: " << res << " files" << endl;
    } else if (key == "SCTTRK_MODE") {
      int ival;
      sline >> ival;
      ftkset.setSCTtrkMode(ival);
    } else if (key == "SCTTRK_TRACKLIST") {
      sline >> scttrk_tracklist;
    } else if (key == "SCTTRK_ROADLIST") {
      sline >> scttrk_roadlist;
    } else if (key == "SCTTRK_SECTORMAP") {
      string sval;
      sline >> sval;
      // sector map is only needed in SCTtrk mode
      if (ftkset.getSCTtrkMode()) {
	scttrk_sectormap = new FTKSectorMap(sval.c_str());
      }
    } else if (key == "SCTTRK_NLAYERS") {
      int ival;
      sline >> ival;
      rfobj.setSCTtrkNlayers(ival);
    } else if (key == "SCTTRK_NSUBS") {
      int ival;
      sline >> ival;
      rfobj.setSCTtrkNsubs(ival);
    } else if (key == "NSUBREGIONS") {
      int ival;
      sline >> ival;
      rfobj.setNsubregions(ival);
    } else if (key == "SCTTRK_LASTLAYER") {
      int ival;
      sline >> ival;
      rfobj.setSCTtrkLastLayer(ival);
    } else if (key == "SCTTRK_MAX_MISSING_PLANES") {
      int ival;
      sline >> ival;
      rfobj.setSCTtrkMaxMissingPlanes(ival);
    } else if (key == "CUR_REGION") {
      int ival;
      sline >> ival;
      CUR_REGION = ival;
    } else if (key == "CUR_SUBREGION") {
      int ival;
      sline >> ival;
      rfobj.setSubregion(ival);
    } else if (key == "REQUIRE_FIRST") {
      bool ival;
      sline >> ival;
      require_first = ival;
    } else if (key == "FORCE_AM_HASHMAP") {
      bool ival;
      sline >> ival;
      force_am_hashmap = ival;
    } else if (key == "ENCODE_SUBREGION") {
      int ival;
      sline >> ival;
      rfobj.setEncodeSubregion(ival);
    } else if (key == "MAKECACHE") {
      sline >> doMakeCache;
    } else if (key == "CACHEPATH") {
      sline >> CachePath;
    } else if (key == "NBANKS") {
      int ival;
      sline >> ival;
      // set the number of banks
      rfobj.setNBanks(ival);
    } else if (key == "USETSP_BANK") {
      // 0 - not use the TSP ready bank, >0 use the TSP bank
      sline >> useTSPBank;
    } else if (key == "USETSP_DBLEVEL") {
	// choose the level: 
	// 1 - best TSP level,, 2 - 1st group, 3 - 2nd group....
      sline >> BankLevel;
    } else if (key == "USETSP_SIM") {
      /* 0 -  use the TSP bank but only as AM, 
	 1 - build the AM related without further steps
	 2 - build the AM related bank and the DC mask
	 3 - simulate the TSP
       */
      sline >> doTSPSim;
    } else if (key == "USETSP_MINCOVERAGE") {
      /* If this value is >1 the AM is built using all the TSP
	 patterns with the required coverage, than the AM bank is
	 cutted
       */      
      sline >> minTSPCoverage;
    } else if (key == "SAVEALLROADS") {
      sline >> SaveAllRoads;      
    } else if (key == "BADMOD_FILE") {  // set the bad module map path 
      sline >> m_badmap_path;
    } else if (key == "BADMOD_FILE_FOR_HIT") {  // set the bad module map path 
      sline >> m_badmap_path2;
    } else if (key == "BANK") {
      int ibank, maxpatt;
      string bankpath;
      /* retrieve the number of the bank the tha max number 
	 of patterns to read (-1 means all patterns) */
      sline >> ibank >> maxpatt;
      // the next line is the bank path
      replace_getline(cin, bankpath);
      FTK_AMBank *bank = 0x0;
      if (useTSPBank) {
	FTKTSPBank *tspbank = new FTKTSPBank(ibank);
	// set the TSP variables
	tspbank->setSimulateTSP(doTSPSim);
	tspbank->setSSMapTSP(ssmap_tsp);
	tspbank->setTSPMinCoverage(minTSPCoverage);
	tspbank->setMakeCache(doMakeCache>0 ? true : false);
	tspbank->setCachePath(CachePath.c_str());
	bank = tspbank;
      }
      else bank = new FTK_AMBank(ibank);
    
      /* if this is set in a TSP simulation also the roads rejected 
	 by the DC mechanism or by the TSP are recorded. In this case
	 the number of hits is correctly updated to the number of verified
	 layers */
      bank->setSaveAllRoads(SaveAllRoads);
      bank->setRequireFirst(require_first);
      // additional adjustments for SCTtrk mode
      if (ftkset.getSCTtrkMode()) {
	// use STL hashmap version of ss->[patterns] lookup table
	bank->enableLookupMap();
	// always require SCTtrk layer in AM matching
	bank->setRequireLast(true);
      }
      if (force_am_hashmap) {
	bank->enableLookupMap();
      }
      ssmap->setOffsetFraction(ss_offset_fraction);
      bank->setSSMap(ssmap);
      bank->setSSMapUnused(ssmap_unused);

      // set the bad module map path   
      bank->setBadSSMapPath ( m_badmap_path ); 
      bank->setBadSSMapPath2 ( m_badmap_path2 ); 
      
      
      if (bankpath.substr(bankpath.size() - 2).compare("db") == 0) {
	if (bank->readDBBank(bankpath.c_str(), maxpatt, BankLevel) < 0) {
	  delete bank;
	  return -1;
	}
      } else if (bankpath.substr(bankpath.size() - 4) == string("root")) {
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
      rfobj.setAMBank(ibank, bank);
      ftkset.usageStat();
    } else if (key == "RAW_MODE") {
      int ival;
      sline >> ival;
      ftkset.setRawMode(ival);
      FTK_RawInput *imod(0);
      switch (ival) {
      case 1:
	if (!pmap) {
	  cout << "*** RAW data input module needs a valid PMap"
	       << endl;
	  cout << "*** Call PMAP before RAW_MODE" << endl;
	  return -1;
	}
	imod = new FTK_RawInput(pmap);
	rfobj.setDataInputModule(imod);
	break;
      case 2:
	if (!pmap) {
	  cout << "*** RAW data input module needs a valid PMap"
	       << endl;
	  cout << "*** Call PMAP before RAW_MODE" << endl;
	  return -1;
	}
	//pmap_unused = new FTKPlaneMap(pmap->getPath(),true);
	imod = new FTK_RawInput(pmap, pmap_unused);
	imod->setSaveUnused(true);
	rfobj.setDataInputModule(imod);
	rfobj.setSSSearchUnused(true);
	break;
      default:
	cout << "*** RAW_MODE=" << ival << " is not supported" << endl;
	return -1;
      }
    } else if (key == "PMAP_FILE") {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap = new FTKPlaneMap(sval.c_str());
      if (!(*pmap)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	return -1;
      }
    } else if (key == "PMAP_FILE_UNUSED") {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap_unused = new FTKPlaneMap(sval.c_str());
      if (!(*pmap_unused)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	return -1;
      }
    } else if (key == "SCT_PAIR") {
      int ival;
      sline >> ival;
      if (pmap) {
	pmap->setEnablePlanePair(ival);
      } else {
	cerr
	  << "*** impossible to define SCT_PAIR variable, PMAP not instantiated"
	  << endl;
	return -1;
      }
    }
    // First key is for backwards compatibility
    else if (key == "ENABLESCTMISSPAIR" || key == "MAX_MISSING_SCT_PAIRS") {
      int ival;
      sline >> ival;
      ftkset.setMaxMissingSctPairs(ival);
    } else if (key == "RESTRICT_SCT_PAIR_LAYER") {
      int ival;
      sline >> ival;
      ftkset.setRestrictSctPairLayer(ival);
    } else if (key == "RESTRICT_SCT_PAIR_MODULE") {
      int ival;
      sline >> ival;
      ftkset.setRestrictSctPairModule(ival);
      if (ival)
	// This implies RESTRICT_SCT_PAIR_LAYER
	ftkset.setRestrictSctPairLayer(ival);
    } else if (key == "RMAP_FILE") {
      // set rmap file
      string sval;
      sline >> sval;
      rmap = new FTKRegionMap(pmap, sval.c_str());
      if (!(*rmap)) {
	cerr << "*** error using map from: " << sval << endl;
	return -1;
      }
    } else if (key == "DIAG_CLUSTERING") {
      int ival;
      sline >> ival;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      
      // use dynamic casting to guess the input type
      // at the moment is over-shooting
      if (FTK_RawInput *cinput = dynamic_cast<FTK_RawInput*>(dinput)) {
	if (ival > 0) {
	  cinput->setDiagClustering(true);
	} else {
	  cinput->setDiagClustering(false);
	}
      }
    } else if (key == "SCT_CLUSTERING") {
      int ival;
      sline >> ival;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      
      if (ival > 0) {
	dinput->setSctClustering(true);
      } else {
	dinput->setSctClustering(false);
      }

    } else if (key == "DUPLICATE_GANGED") {
      int ival;
      sline >> ival;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      
      // use dynamic casting to guess the input type
      // at the moment is over-shooting
      if (FTK_RawInput *cinput = dynamic_cast<FTK_RawInput*>(dinput)) {
	if (ival > 0) {
	  cinput->setDuplicateGanged(true);
	} else {
	  cinput->setDuplicateGanged(false);
	}
      }
    } else if (key == "GANGED_PATTERN_RECOGNITION") {
      int ival;
      sline >> ival;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      
      // use dynamic casting to guess the input type
      // at the moment is over-shooting
      if (FTK_RawInput *cinput = dynamic_cast<FTK_RawInput*>(dinput)) {
	if (ival > 0) {
	  cinput->setGangedPatternRecognition(true);
	} else {
	  cinput->setGangedPatternRecognition(false);
	}
      }
    } else if (key == "SPLIT_BLAYER_MODULES") {
      int ival;
      sline >> ival;
      FTKDataInput *dinput = rfobj.getDataInputModule();
      
      // use dynamic casting to guess the input type
      // at the moment is over-shooting
      if (FTK_RawInput *cinput = dynamic_cast<FTK_RawInput*>(dinput)) {
	if (ival > 0) {
	  cinput->setSplitBlayerModules(true);
	} else {
	  cinput->setSplitBlayerModules(false);
	}
      }
    } else if (key == "MULT_OUT") {
      int ival;
      sline >> ival;
      // set if the output module saves an outfile
      // for each input file, works only with ascii input data
      FTKRoadOutput *outmode = rfobj.getRoadOutputModule();
      if (ival)
	outmode->setMultiOut(true);
      else
	outmode->setMultiOut(false);
    } else if (key == "OUT_FILE") {
      string sval;
      sline >> sval;
      FTKRoadOutput *outmode = rfobj.getRoadOutputModule();
      outmode->setFileName(sval.c_str());
    } else if (key == "OUT_DIR") {
      string sval;
      sline >> sval;
      FTKRoadOutput *outmode = rfobj.getRoadOutputModule();
      outmode->setOutDir(sval.c_str());
    } else {
      cout << prompt << "\"" << key << "\" is not a valid key" << endl;
      continue;
    }
  }
  
  // Initialize 8L SCTtrk road and track input
  if (ftkset.getSCTtrkMode()) // cy
    {
      if (!scttrk_sectormap) {
	cout << "SCTTRK_SECTORMAP was not set in the config file" << endl;
	exit(0);
      }
      rfobj.setSectorMap(scttrk_sectormap);
      FTKDataInput *dinput = rfobj.getDataInputModule();
      if (!rmap) {
	cout << "*** SCTTRK_LIST data input module needs a valid RMap"
	     << endl;
	return -1;
      }
      if (CUR_REGION == -1) {
	cout << "*** Set CUR_REGION in order to load 8L SCTtrk roads"
	     << endl;
	return -1;
      }
      // Sanity check
      if (ftkset.getMaxMissingSctPairs() > rfobj.getSCTtrkMaxMissingPlanes()) {
	cout
	  << "*** WARNING: MAX_MISSING_SCT_PAIRS > SCTTRK_MAX_MISSING_PLANES"
	  << endl << "*** Setting SCTTRK_MAX_MISSING_PLANES = "
	  << ftkset.getMaxMissingSctPairs() << endl;
	rfobj.setSCTtrkMaxMissingPlanes(ftkset.getMaxMissingSctPairs());
      }
      dinput->useRoadsTracks(rmap->getNumRegions(), CUR_REGION);
      int res = dinput->addTrackFilesList(CUR_REGION,
					  scttrk_tracklist.c_str());
      cout << "Added: " << res << " track files" << endl;
      res = dinput->addRoadFilesList(CUR_REGION, scttrk_roadlist.c_str());
      cout << "Added: " << res << " road files" << endl;
      dinput->initRoadsTracks();
    }
  // Sanity check for the default case
  else if (ftkset.getMaxMissingSctPairs() > ftkset.getMaxMissingPlanes()) {
    cout << "*** WARNING: MAX_MISSING_SCT_PAIRS > MAX_MISSING_PLANES"
	 << endl << "*** Setting MAX_MISSING_PLANES = "
	 << ftkset.getMaxMissingSctPairs() << endl;
    ftkset.setMaxMissingPlanes(ftkset.getMaxMissingSctPairs());
  }
  
  return 0;
}

// double expChi2f (double *x, size_t dim, void *params){
//   return exp(-chi2f(x, dim, params)/2.);
// }


double chi2f (double *x, size_t dim, void *params){
  double chi2(0);
  
  for(int i=0;i<nconstr;++i)
    {
      double s = kaverage[i];
      for(int j=0;j<ncoords;++j) {
	s += kernel[i][j]*x[j];
      }
      chi2 += s*s;
    }

  return chi2;
}
