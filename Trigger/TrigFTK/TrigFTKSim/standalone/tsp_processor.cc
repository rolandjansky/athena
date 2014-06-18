/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  This executable simulates TSP (Tree Search Processor)
  TSP produces a list of roads+hits with SS size reduced in factors of 2

  THIS CODE DOESN'T DO ANYTHING YET - WORK IN PROGRESS!!

*/

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

// number of the regions
int nregions(0);
// number of event files
int nfiles(0);
// number of events to process
int total_nevts(-1);
// Plane-map pointer, the RW should be less effective in the end-caps
FTKPlaneMap *pmap(0);

// list of the regions to skip
set<int> regions_blacklist;

// lists with path of the files to merge for each sub-regions
vector<string> *pathlist(0);

// maximum number of missing SS
int max_missing(0);
// >0 if the removed roads are just marked, if =0 they are removed from the output
int keep_roads(0);

// output basedir
string outdir("");

int read_commands();

void apply_filter(FTKRoadStream *&,FTKRoadStream*);

bool debug_event(false);

/** this program read N files for each groups, for few regions,
    producing a single output file, or set of files */
int main(int argc, char *argv[])
{

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

  // total number of event counter
  int tot_evt(0);

  // loop over the files
  cout << "Loop over # " << nfiles << " files" << endl;
  for (int ifile=0;ifile<nfiles;++ifile) {
    cout << "Open original trees" << endl;
    TFile *srfile[nregions];
    TTree *srtree[nregions];
    FTKRoadStream *srbanks[nregions];

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
      cout << "Opening: " << pathlist[ib][ifile] << endl;
      srfile[ib] = TFile::Open(pathlist[ib][ifile].c_str());
      srtree[ib] = (TTree*) srfile[ib]->Get("ftkdata");
      if (!srtree[ib]) {
	cerr << "*** Error: TTree in file \"" << pathlist[ib][ifile] << "\" not found" << endl;
	return -1;
      }
      srbanks[ib] = new FTKRoadStream();
      srtree[ib]->SetBranchAddress(Form("FTKBank%d.",ib),&srbanks[ib]);

      // assume the standard number of events from the first TTree
      if (nevents==-1) {
	nevents = srtree[ib]->GetEntries();
	merged_fname = gSystem->BaseName(pathlist[ib][ifile].c_str());
      }
    } // end region loop

    // create the merged file
    string ofname = Form("%s/%s",outdir.c_str(),merged_fname.c_str());
    TFile *ofile = TFile::Open(ofname.c_str(),"recreate");
    
    // create the TTree and the branches
    TTree *tree = new TTree("ftkdata","FTK Simulation Results (after TSP)");
    // recent bank object
    FTKRoadStream **banks = new FTKRoadStream*[nregions];
    for (int ib=0;ib<nregions;++ib) {
      banks[ib] = new FTKRoadStream();
      tree->Branch(Form("FTKBank%d.",ib),&banks[ib],TREE_ROAD_BUFSIZE);
    }


    int iev_step = nevents/100;
    cout << "Begin loop over # " << nevents << " events" << endl;
    for (int iev=0;iev<nevents&&tot_evt!=total_nevts;++iev,++tot_evt) { // event loop
      if (iev_step!=0 && iev%iev_step==0) {
	cout << "Event " << iev << endl;
	debug_event = true;
      }
      else if (debug_event) {
	debug_event = false;
      }

      for (int ib=0;ib<nregions;++ib) { // bank loop
	// check if the region has to be skipped
	if (regions_blacklist.find(ib)!=regions_blacklist.end())
	  continue;

	srtree[ib]->GetEntry(iev);
	if (iev==0) {
	  banks[ib]->init(srbanks[ib]->getNPlanes());
	}
	
	// clear clones for this bank
	banks[ib]->clear();

	// apply the road and hit filter
	apply_tsp(banks[ib],srbanks[ib]);

      } // end bank loop


      // fill the completed results
      tree->Fill();
    } // end event loop

    // write, close, and destroy the file
    ofile->Write();
    ofile->Close();
    delete ofile;
  } // end loop over files

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
      pathlist = new vector<string>[nregions];
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
      sline >> regid;
      // red the following lines, with a number equal with the number of
      // the files
      string path;
      for (int i=0;i<nfiles;++i) {
	if (getline(cin,path)) {
	  pathlist[regid].push_back(path);
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
  Performs road and hit filtering
*/
void apply_tsp(FTKRoadStream * &newbank,FTKRoadStream *oldbanks)
{
  if (debug_event) {
    cout << "Applying TSP logic: ";
  }

  // TODO
  
  if (debug_event) {
    cout << endl;
  }
  
}
