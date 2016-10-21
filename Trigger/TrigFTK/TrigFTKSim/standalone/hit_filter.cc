/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  This executable takes merged roads from each shifted bank group
  and performs road filtering + hit filtering.
  The output can be used with the usual track fitter.
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/PatternBank.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/ftkdefs.h"

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TBits.h>

#include <getopt.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
using namespace std;

const int HFDEBUG = 0; // debug variable for hitfilter

// number of the regions
int nregions(0);
// number of the groups for each region
int ngroups(0);
// number of event files
int nfiles(0);
// number of events to process
int total_nevts(-1);
// how many multiple matches in ss-shifted bank for each road in default bank?
unsigned int hf_max_matches(50);
unsigned int hf_topo_matches(0);
// Plane-map pointer, the RW should be less effective in the end-caps
FTKPlaneMap *pmap(0);

// list of the regions to skip
set<int> regions_blacklist;

// lists with path of the files to merge for each sub-regions
vector<string> **pathlist(0);

// maximum number of missing SS
int max_missing(0);
// >0 if the removed roads are just marked, if =0 they are removed from the output
int keep_roads(0);

// output basedir
string outdir("");

int read_commands();

void apply_filter(FTKRoadStream *&,FTKRoadStream**);

bool debug_event(false);

/** this program read N files for each groups, for few regions,
    producing a single output file, or set of files */
int main(int argc, char *argv[])
{
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  ftkset.setVerbosity(0);

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

  if(ngroups!=2) {
    cerr << "*** Currently only ss-1/2 shifted banks are supported. You specified: " << ngroups << endl;
    return -1;
  }
  
  // total number of event counter
  int tot_evt(0);

  // loop over the files
  cout << "Loop over # " << nfiles << " files" << endl;
  for (int ifile=0;ifile<nfiles;++ifile) {
    cout << "Open original trees" << endl;
    TFile *srfile[nregions][ngroups];
    TTree *srtree[nregions][ngroups];
    FTKRoadStream *srbanks[nregions][ngroups];

    // the number of event will be set by the first valid region
    int nevents(-1);
    // this will become equal the filename of the first valid region
    string merged_fname("merged_file.root");

    // loop to open original TTree
    for (int ib=0;ib<nregions;++ib) { // region loop
      // check if the region has to be skipped
      if (regions_blacklist.find(ib)!=regions_blacklist.end())
	continue;

      // retrieve the path and open the original files
      for (int isb=0;isb<ngroups;++isb) { // sub-region loop
	cout << "Opening: " << pathlist[ib][isb][ifile] << endl;
	srfile[ib][isb] = TFile::Open(pathlist[ib][isb][ifile].c_str());
	srtree[ib][isb] = (TTree*) srfile[ib][isb]->Get("ftkdata");
	if (!srtree[ib][isb]) {
	  cerr << "*** Error: TTree in file \"" << pathlist[ib][isb][ifile] << "\" not found" << endl;
	  return -1;
	}
	srbanks[ib][isb] = new FTKRoadStream();
	srtree[ib][isb]->SetBranchAddress(Form("FTKBank%d.",ib),&srbanks[ib][isb]);
      } // end sub-region loop


      // assume the standard number of events from the first TTree
      if (nevents==-1) {
	nevents = srtree[ib][0]->GetEntries();
	merged_fname = gSystem->BaseName(pathlist[ib][0][ifile].c_str());
      }
    } // end region loop

    // create the merged file
    string ofname = Form("%s/%s",outdir.c_str(),merged_fname.c_str());
    TFile *ofile = TFile::Open(ofname.c_str(),"recreate");
    
    // create the TTree and the branches
    TTree *tree = new TTree("ftkdata","FTK Simulation Results (merged and filtered)");
    // recent bank object
    FTKRoadStream **banks = new FTKRoadStream*[nregions];
    for (int ib=0;ib<nregions;++ib) {
      banks[ib] = new FTKRoadStream();
      tree->Branch(Form("FTKBank%d.",ib),&banks[ib],TREE_ROAD_BUFSIZE);
    }


    int iev_step = nevents>500 ? nevents/100 : 1;
    cout << "Begin loop over # " << nevents << " events" << endl;
    for (int iev=0;iev<nevents&&tot_evt!=total_nevts;++iev,++tot_evt) { // event loop
      if (iev%iev_step==0) {
	cout << "Event " << iev << endl;
	debug_event = true;
      }
      else if (debug_event) {
	debug_event = false;
      }
      if (iev%(iev_step*10)==0)
	ftkset.usageStat();

      for (int ib=0;ib<nregions;++ib) { // bank loop
	// check if the region has to be skipped
	if (regions_blacklist.find(ib)!=regions_blacklist.end())
	  continue;
	
	for (int isb=0;isb<ngroups;++isb) { 
	  srtree[ib][isb]->GetEntry(iev);
	  if (iev==0&&isb==0) {
	    banks[ib]->init(srbanks[ib][isb]->getNPlanes());
	  }
	}
	
	// clear clones for this bank
	banks[ib]->clear();
	
	// apply the road and hit filter
	apply_filter(banks[ib],srbanks[ib]);
	
      } // end bank loop

      // fill the completed results
      tree->Fill();

    } // end event loop
    
    // write, close, and destroy the file
    ofile->Write();
    ofile->Close();
    delete ofile;

    // cleanup allocated memory
    for (int ib=0;ib<nregions;++ib) { // region loop
      delete banks[ib];
      if (regions_blacklist.find(ib)!=regions_blacklist.end())
	continue;
      for (int isb=0;isb<ngroups;++isb) { // sub-region loop
	srfile[ib][isb]->Close();
	delete srbanks[ib][isb];
      }
    }
    delete [] banks;
    
  } // loop over files
}


int read_commands() {
  string line;
  while (getline(cin,line)) { // loop over the configuration file
    istringstream sline(line);
    cout << "PARSE> " << line << endl;
    // skip empty lines and comments
    if (!line.size()) continue;
    else if (line[0]=='#') continue;

    string key;
    sline >> key;

    if (key=="OUTDIR") {
      // set base output directory
      sline >> outdir;
    }
    else if (key=="KEEP_REMOVED") {
      sline >> keep_roads;
    }
    else if (key=="NBANKS") {
      // number of the banks
      sline >> nregions;
      pathlist = new vector<string>*[nregions];
    }
    else if (key=="NGROUPS") {
      sline >> ngroups;
      for (int i=0;i<nregions;++i) {
	pathlist[i] = new vector<string>[ngroups];
      }
    }
    else if (key=="HF_MAX_MATCHES") {
      sline >> hf_max_matches;
    }
    else if (key=="HF_TOPO_MATCHES") {
      sline >> hf_topo_matches;
    }
    else if (key=="SKIPREGIONS") {
      int ival;
      while (1) {
	sline >> ival;
	if (!sline) break;
	regions_blacklist.insert(ival);
      }
    }
    else if (key=="NFILES") {
      sline >> nfiles;
    }
    else if (key=="NEVENTS") {
      sline >> total_nevts;
    }
    else if (key=="PMAP_FILE") {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap = new FTKPlaneMap(sval.c_str());
      if (!(*pmap)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	pmap = 0;
      }
    }
    else if (key=="SCT_PAIR") {
      int ival;
      sline >> ival;
      if (pmap) {
	pmap->setEnablePlanePair(ival);
      }
      else {
	cerr << "*** impossible to define SCT_PAIR variable, PMAP don't instanciated" << endl;
      }
    }
    else if (key=="BANK") {
      int regid;
      int subid;
      sline >> regid >> subid;
      // red the following lines, with a number equal with the number of
      // the files
      string path;
      for (int i=0;i<nfiles;++i) {
	if (getline(cin,path)) {
	  pathlist[regid][subid].push_back(path);
	}
      }
    }
    else {
      cout << "*** unknown directive" << endl;
    }
  } // end loop over the configuration file
  
  return 0;
}

/*
  Checks if the road has a match among shifted-group roads.
  Returns a list of matched roads, or empty list if no matches are found
*/
std::vector<FTKRoad*> find_matching_roads(FTKRoad *r, FTKRoadStream **oldbanks) {
  const int isr = 1; // index of shifted group
  std::vector<FTKRoad *> result;
  
  // loop over roads in ss-shifted bank
  int nroads = oldbanks[isr]->getNRoads();
  for (int iroad=0;iroad<nroads;++iroad) {
    FTKRoad *cur_road = oldbanks[isr]->getRoad(iroad);
    // check that sector is the same
    if(r->getSectorID()!=cur_road->getSectorID()) {
      continue;
    }
    // check that pattern id in each layer is compatible: ss1=ss2 or ss1=ss2+1
    bool good=true;
    int curside(-1),nshifts(0);
    for(int pl=0; pl<r->getNPlanes(); pl++) {
      if(r->getSSID(pl)==cur_road->getSSID(pl)) {
	if(curside!=0) nshifts++;
	curside=0;
	continue;
      }
      if (r->getSSID(pl)==cur_road->getSSID(pl)+FTKSSMap::getPhiOffset(pmap->isSCT(pl),false) ) {
	if(curside!=1) nshifts++;
	curside=1;
	continue;
      }
      good = false;
      break;
    }
    if(!good) continue;
    /*
      enforce additional topological matching criteria:
      require there are no cross-matches (likely caused by noise):
      L3: ss1==ss2
      L2: ss1==ss2
      L1: ss1==ss2+1
      L0: ss1==ss2 - BAD, this is a cross-match
    */

    if(HFDEBUG) {
      std::cout << "shifts = " << nshifts << std::endl;
    }
    // nshifts: sum=566328 n=122296 <avg>=4.6308
    if(hf_topo_matches && nshifts>2) continue;

    // if we are here, we found a matching road!
    result.push_back(cur_road);
    // break if enough matches have been found already
    if(result.size()>=hf_max_matches) break;
  }
  return result;
}

/*
  returns per-layer bitmap for road "r" that marks hits not found in ss-shifted road "m".
*/
std::vector<TBits> get_hf_bitmask(FTKRoad *r, FTKRoad *m) {
  
  std::vector<TBits> b(r->getNPlanes());

  if(HFDEBUG) {
    std::cout << "CONSIDERING ROAD:" << std::endl;
    std::cout << (*r) << std::endl;
    std::cout << "MATCHING ROAD:" << std::endl;
    std::cout << (*m) << std::endl;
  }
  
  // compare hits in roads "r"[oad] and "m"[atch] in each plane's superstrip
  for(int pl=0; pl<r->getNPlanes(); pl++) {
    // missing plane - nothing to mark in bitmask
    if(r->getNHits(pl)==0) continue;
    // if "m" road had no hits, don't filter anything in this plane
    if(m->getNHits(pl)==0) continue;
    // loop over all hits in this plane's SS for road "r"
    for(unsigned int ir=0; ir < r->getHits(pl).size(); ir++) {
      bool found(false);
      // loop over all hits in this plane's SS for matching road "m"
      for(unsigned int im=0; im < m->getHits(pl).size(); im++) {
	if (r->getHit(pl,ir)==m->getHit(pl,im) ) {
	  found=true;
	  break;
	}
      }
      if(!found) b[pl].SetBitNumber(ir);
    }
    if(HFDEBUG) {
      std::cout << "plane " << pl << ": " <<  std::endl; 
      b[pl].Print();
    }
    b[pl].Compact();
  }

  return b;
}

/*
  Performs road and hit filtering
*/
void apply_filter(FTKRoadStream * &newbank,FTKRoadStream **oldbanks)
{
  if (debug_event) {
    cout << "Roads considered by hit filter: " << oldbanks[0]->getNRoads() << " / " << oldbanks[1]->getNRoads();
  }
  const int isr = 0; // index of non-shifted (default) group

  if(HFDEBUG) {
    if(oldbanks[0]->getNRoads() || oldbanks[1]->getNRoads()) {
      std::cout << std::endl;
      std::cout << "DEFAULT: " << oldbanks[0]->getNRoads() << std::endl;
      oldbanks[0]->Print(true);
      std::cout << "SHIFTED: " << oldbanks[1]->getNRoads() << std::endl;
      oldbanks[1]->Print(true);
      std::cout << std::endl;
    }
  }

  // Attach hits to all roads in the event [need to check memory reqs in 10^34 events]
  for(int igroup=0; igroup<ngroups; igroup++) {
    int nroads = oldbanks[igroup]->getNRoads();
    for (int iroad=0;iroad<nroads;++iroad) {
      oldbanks[igroup]->attachHits(iroad);
    }
  }

  // copy run and event number to the output, and check that the
  // groups are in sync.
  unsigned long grp_run = oldbanks[isr]->runNumber();
  unsigned long grp_event = oldbanks[isr]->eventNumber();
  newbank->setRunNumber( grp_run );
  newbank->setEventNumber( grp_event );
  if( grp_run!=0 || grp_event!=0 ) {
    bool event_sync = true;
    for(int igrp=0; igrp!=ngroups; ++igrp) {
      unsigned long this_run = oldbanks[igrp]->runNumber();
      unsigned long this_event = oldbanks[igrp]->eventNumber();
      if( this_run != grp_run || this_event != grp_event ) {
        event_sync = false;
        break;
      }
    }
    if( !event_sync ) {
      std::cout << "DEFAULT AND SHIFTED BANKS ARE OUT OF SYNC!!!" << std::endl;
      std::cout << "Group\t\tRun\t\tEvent!" << std::endl;
      for(int igrp=0; igrp!=ngroups; ++igrp) {
        std::cout << igrp << "\t\t" << oldbanks[igrp]->runNumber() << "\t\t" << oldbanks[igrp]->eventNumber() << std::endl;
      }
      return;
    }
  }
    
  int nroads = oldbanks[isr]->getNRoads();

  // make the following map: roadIDmap[bankID] = max roadID in each bankID
  // we use it to get the next available index for newly created roads.
  std::map<int,int> roadIDmap;
  // loop over roads in non-shifted bank
  for (int iroad=0;iroad<nroads;++iroad) {
    FTKRoad *cur_road = oldbanks[isr]->getRoad(iroad);
    int bid = cur_road->getBankID();
    int rid = cur_road->getRoadID();
    if (roadIDmap.find(bid)==roadIDmap.end()) roadIDmap[bid]=0;
    if(rid>roadIDmap[bid]) roadIDmap[bid]=rid;
  }
  
  // loop over roads in non-shifted bank
  for (int iroad=0;iroad<nroads;++iroad) {
    FTKRoad *cur_road = oldbanks[isr]->getRoad(iroad);
    // get the list of matching roads from ss-shifted bank:
    std::vector<FTKRoad*> m = find_matching_roads(cur_road,oldbanks);
    // if no matches found, mark the road as rejected and save it
    if(m.size()==0) {
      FTKRoad *new_road = newbank->addRoad(*cur_road);
      new_road->setHFRejected(1);
      continue;
    }
    // if matches are found, create a new road for each match (with corresponding bitmask)
    for(unsigned int i=0; i<m.size(); i++) {
      // save a copy of this road
      FTKRoad *new_road = newbank->addRoad(*cur_road);
      new_road->setHFStatus(i);
      // 1st instance keeps id of parent; others get completely new id's
      if(i>0) {
	int bid = cur_road->getBankID();
	new_road->setRoadID(++roadIDmap[bid]);
      }
      // set bitmask for this copy
      new_road->setHitsMask( get_hf_bitmask(cur_road,m[i]) );
    }
  }
  
  // Detach hits from all roads - both for input and output road streams
  for(int igroup=0; igroup<ngroups; igroup++) {
    int nroads = oldbanks[igroup]->getNRoads();
    for (int iroad=0;iroad<nroads;++iroad) {
      oldbanks[igroup]->detachHits(iroad);
    }
  }
  for (int iroad=0;iroad<newbank->getNRoads();++iroad) {
    newbank->detachHits(iroad);
  }

  // for now, just copy over the FTKSS container [we could actually slim it a little]
  const FTKSS_container_t &strips_cont = oldbanks[isr]->getSSContainer();
  int cont_nplanes = strips_cont.size();
  for (int ipl=0;ipl!=cont_nplanes;++ipl) { // plane loop
    const FTKSS_map_t &ssmap = strips_cont[ipl];
    FTKSS_map_t::const_iterator iss = ssmap.begin();
    for (;iss!=ssmap.end();++iss) { // SS loop
      newbank->addSS(ipl,(*iss).first,(*iss).second);
    } // end SS loop
  } // end plane loop

  // clean up HF rejected roads (if requested)
  int totroads = newbank->getNRoads();  
  if(!keep_roads) {
    for (int imr=0;imr<totroads;++imr) {
      FTKRoad *ref = newbank->getRoad(imr);
      if (ref->getHFRejected()) {
	newbank->removeRoad(imr);
      }
    }
    // clean the array of the roads in the bank to
    // remove the holes leaved by the rejected roads
    newbank->optimize();
  }
  
  if (debug_event) {
    cout << endl;
  }
  
}
