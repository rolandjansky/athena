/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/PatternBank.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKRoad.h"
#include "TrigFTKSim/ftkdefs.h"
#include "common_fcn.h"

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>

#include <getopt.h>
#include <unistd.h>  // for sleep()
#define SECTOR_LOAD_SLEEP 3

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <sstream>
using namespace std;

// number of the regions
int nregions(0);
// number of the subregions for each region
int nsubregions(0);
// whether subregion info should be encoded into bankID variable (100*isub+ireg)
int ENCODE_SUBREGION(1);
// number of event files
int nfiles(0);
// number of event to merge
int total_nevts(-1);
// Plane-map pointer, the RW should be less effective in the end-caps
FTKPlaneMap *pmap(0);

// sector definition banks, patterns and sectors banks have the same format
PatternBank ***sectbank(0);

// list of the regions to skip
set<int> regions_blacklist;

// lists with path of the files to merge for each sub-regions
vector<string> **pathlist(0);

// maximum number of missing SS
int max_missing(0);
// decide the maximum Road-Warrior level
// the default is the maximum level
int road_warrior(0);
// >0 if the removed roads are just marked, if =0 they are removed from the output
int keep_roads(0);
// delete road files after they are not needed anymore (saves disk space)
bool delete_roads_after_use(0);

// output basedir
string outdir("");

int read_commands();

bool informationMatch(const FTKRoad &, const FTKRoad &);

void merge_roads(FTKRoadStream *&,FTKRoadStream**);


bool debug_event(false);

/** this program read N files for each subregions, for few regions,
    producing a single output file, or set of files */
int main(int argc, char *argv[])
{
  common_fcn_init(argc,argv);

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

  // total number of event counter
  int tot_evt(0);

  // loop over the files
  cout << "Loop over # " << nfiles << " files" << endl;
  for (int ifile=0;ifile<nfiles;++ifile) {
    cout << "Open original trees" << endl;
    TFile *srfile[nregions][nsubregions];
    TTree *srtree[nregions][nsubregions];
    TBranch *srbranch[nregions][nsubregions];
    FTKRoadStream *srbanks[nregions][nsubregions];

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
      for (int isb=0;isb<nsubregions;++isb) { // sub-region loop
	cout << "Opening: " << pathlist[ib][isb][ifile] << endl;
	srfile[ib][isb] = TFile::Open(pathlist[ib][isb][ifile].c_str());
	srtree[ib][isb] = (TTree*) srfile[ib][isb]->Get("ftkdata");
	if (!srtree[ib][isb]) {
	  cerr << "*** Error: TTree in file \"" << pathlist[ib][isb][ifile] << "\" not found" << endl;
	  return -1;
	}
	srbanks[ib][isb] = new FTKRoadStream();
	srtree[ib][isb]->SetBranchAddress(Form("FTKBank%d.",ib),&srbanks[ib][isb],&srbranch[ib][isb]);
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
    TTree *tree = new TTree("ftkdata","FTK Simulation Results (merged)");
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

	for (int isb=0;isb<nsubregions;++isb) { 
	  srbranch[ib][isb]->GetEntry(iev);
	  if (iev==0&&isb==0) {
	    banks[ib]->init(srbanks[ib][isb]->getNPlanes());
	  }
	}

	// clear clones for this bank
	banks[ib]->clear();
	
	// update roads[ib] summing, after the RW, the original roads
	merge_roads(banks[ib],srbanks[ib]);

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
      for (int isb=0;isb<nsubregions;++isb) { // sub-region loop
	srfile[ib][isb]->Close();
	delete srbanks[ib][isb];
	// non-reversible cleanup of road input files
	if(delete_roads_after_use) {
	  std::cout << "Warning: deleting file " << pathlist[ib][isb][ifile] << std::endl;
	  gSystem->Unlink(pathlist[ib][isb][ifile].c_str());
	}
      }
    }
    delete [] banks;
    
  } // loop over files
}


int read_commands() {
  string line;
  while (replace_getline(cin,line)) { // loop over the configuration file
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
    else if (key=="MAX_MISSING_PLANES") {
      // set number of missing SS
      sline >> max_missing;
    }
    else if (key == "ROADWARRIOR") {
       sline >> road_warrior;
    }
    else if (key=="KEEP_REMOVED") {
      sline >> keep_roads;
    }
    else if (key=="NBANKS") {
      // number of the banks
      sline >> nregions;
      pathlist = new vector<string>*[nregions];
      sectbank = new PatternBank**[nregions];
    }
    else if (key=="NSUBREGIONS") {
      sline >> nsubregions;
      for (int i=0;i<nregions;++i) {
	pathlist[i] = new vector<string>[nsubregions];
	sectbank[i] = new PatternBank*[nsubregions];
      }
    }
    else if (key=="SKIPREGIONS") {
      int ival;
      while (1) {
	sline >> ival;
	if (!sline) break;
	if (ival>=0) { // should be a valid region number
	  regions_blacklist.insert(ival);
	}
	else { // means all the regions
	  for (int ir=0;ir!=nregions;++ir) {
	    regions_blacklist.insert(ir);
	  }
	}
      }
    }
    else if (key=="NOSKIPREGIONS") {
      int ival;
      while (1) {
	sline >> ival;
	if (!sline) break;
	regions_blacklist.erase(ival);
      }
    }
    else if (key=="NFILES") {
      sline >> nfiles;
    }
    else if (key=="NEVENTS") {
      sline >> total_nevts;
    }
    else if (key=="DELETE_ROADS_AFTER_USE") {
      sline >> delete_roads_after_use;
    }
    else if (key=="ENCODE_SUBREGION") {
      sline >> ENCODE_SUBREGION;
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
      // variable used to decide if the bank declaration
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

      for (int ir=regid;ir!=nregions;++ir) { // region loop
	cin.seekg(fpos);

	// check if the region has to beskipped
	if (regions_blacklist.find(ir)!=regions_blacklist.end())
	  continue;

	for (int is=subid;is!=nsubregions;++is) { // sub-region loop
	  cin.seekg(fpos);

	  // red the following lines, with a number equal with the number of
	  // the files
	  string path;
	  if (replace_getline(cin,path)) {
	    if (doRegLoop) {
	      replace_ids(path,model_reg,ir);
	    }
	    if (doSubLoop) {
	      replace_ids(path,model_sub,is);
	    }

	    if (doRegLoop||doSubLoop) {
	      cout << "Load bank [" << ir << "," << is << "]: " << path << endl;
	    }

	    // load sectors - but only if overlap roadwarrior is used
	    if(road_warrior>=3) {
	      sectbank[ir][is] = new PatternBank(path.c_str());
	      // For files on PNFS, quickly loading sectors will crash DCACHE server
	      // To remedy this, we "sleep" a little:
	      if(path.find("pnfs")!=std::string::npos) {
		std::cout << "Sleeping 1st time for " << SECTOR_LOAD_SLEEP << " seconds " << std::endl;
		sleep(SECTOR_LOAD_SLEEP);
	      }
	      if(!sectbank[ir][is]->InitPreload()) {
		// sectors couldn't be loaded - disable overlap RW
		std::cout << "Warning: disabling overlap roadwarrior" << std::endl;
		road_warrior=2;
	      }
	      if(path.find("pnfs")!=std::string::npos) {
		std::cout << "Sleeping 2nd time for " << SECTOR_LOAD_SLEEP << " seconds " << std::endl;
		sleep(SECTOR_LOAD_SLEEP);
	      }
	    }
	  }

	  for (int i=0;i<nfiles;++i) {
	    if (replace_getline(cin,path)) {
	      if (doRegLoop) {
		replace_ids(path,model_reg,ir);
	      }
	      if (doSubLoop) {
		replace_ids(path,model_sub,is);
	      }

	      if (doRegLoop||doSubLoop) {
		cout << "Insert path: " << path << endl;
	      }
	      
	      pathlist[ir][is].push_back(path);
	    }
	  }

	  if (!doSubLoop) break;
	} // end sub-region loop

	if (!doRegLoop) break;
      } // end region loop
    }
    else {
      cout << "*** unknown directive" << endl;
    }
  } // end loop over the configuration file
  
  return 0;
}


void merge_roads(FTKRoadStream * &newbank,FTKRoadStream **oldbanks)
{
  if (debug_event) {
    cout << "Merging road from sub-region: ";
  }

  // copy run and event number to the output, and check that the
  // subregions are in sync.
  unsigned long sr_run = 0ul;
  unsigned long sr_event = 0ul;
  for (int isr=0;isr!=nsubregions;++isr) { // subregions loop
    unsigned long this_run = oldbanks[isr]->runNumber();
    unsigned long this_event = oldbanks[isr]->eventNumber();
    if( this_run==0 && this_event==0 ) { continue; }
    sr_run = this_run;
    sr_event = this_event;
    break;
  }
  newbank->setRunNumber( sr_run );
  newbank->setEventNumber( sr_event );
  if( sr_run!=0 || sr_event!=0 ) {
    bool event_sync = true;
    for(int isr=0; isr!=nsubregions; ++isr) {
      unsigned long this_run = oldbanks[isr]->runNumber();
      unsigned long this_event = oldbanks[isr]->eventNumber();
      if( this_run != sr_run || this_event != sr_event ) {
        event_sync = false;
        break;
      }
    }
    if( !event_sync ) {
      std::cout << "SUBREGION BANKS ARE OUT OF SYNC!!!" << std::endl;
      std::cout << "Subregion\t\tRun\t\tEvent!" << std::endl;
      for(int isr=0; isr!=nsubregions; ++isr) {
        std::cout << isr << "\t\t" << oldbanks[isr]->runNumber() << "\t\t" << oldbanks[isr]->eventNumber() << std::endl;
      }
      return;
    }
  }
  
  // merge together the roads from different regions or subregions
  for (int isr=0;isr<nsubregions;++isr) { // subregions loop
    if (debug_event) cout << " " << isr << flush;

    // merge statistical informations
    if(isr==0) {
      // total # of hits and clusters is the same in each subregion, so we do it once
      newbank->naoSetNhitsTot(oldbanks[isr]->naoGetNhitsTot());
      newbank->naoSetNclusTot(oldbanks[isr]->naoGetNclusTot());
      // initialize counters from the first subregion
      newbank->naoSetNclus(oldbanks[isr]->naoGetNclus());
      newbank->naoSetNss(oldbanks[isr]->naoGetNss());
      newbank->naoSetNroadsAM(oldbanks[isr]->naoGetNroadsAM());
      newbank->naoSetNroadsMOD(oldbanks[isr]->naoGetNroadsMOD());
    }
    else {
      // increment counters from other subregions
      newbank->naoAddNroadsAM(oldbanks[isr]->naoGetNroadsAM());
      newbank->naoAddNroadsMOD(oldbanks[isr]->naoGetNroadsMOD());
    }

    int nroads = oldbanks[isr]->getNRoads();
    for (int iroad=0;iroad<nroads;++iroad) {
      FTKRoad *cur_road = oldbanks[isr]->getRoad(iroad);
      FTKRoad *new_road = newbank->addRoad(*cur_road);
      // the sub-region ID is hidden in this formula sub*100+reg
      new_road->setBankID(100*isr*ENCODE_SUBREGION+cur_road->getBankID());
    }
    newbank->inc4LRoad(oldbanks[isr]->getN4LRoads());

    // add the SS for all the layers
    const FTKSS_container_t &strips_cont = oldbanks[isr]->getSSContainer();
    int cont_nplanes = strips_cont.size();

    for (int ipl=0;ipl!=cont_nplanes;++ipl) { // plane loop
      const FTKSS_map_t &ssmap = strips_cont[ipl];

      FTKSS_map_t::const_iterator iss = ssmap.begin();
      for (;iss!=ssmap.end();++iss) { // SS loop
	newbank->addSS(ipl,(*iss).first,(*iss).second);
      } // end SS loop
    } // end plane loop

    // add te SS strips in the layers ignored in the RF step
    const map< int, map<int,FTKSS> > &unusedStrips_cont = oldbanks[isr]->getUnusedSSMap();
    map< int, map<int,FTKSS> >::const_iterator uSS_iplane = unusedStrips_cont.begin();
    for (;uSS_iplane!=unusedStrips_cont.end();++uSS_iplane) { // unused plane loop
      const int &plane_id = (*uSS_iplane).first;
      map<int,FTKSS>::const_iterator uSS_iss = (*uSS_iplane).second.begin();
      for (;uSS_iss!=(*uSS_iplane).second.end();++uSS_iss) { // SS loop
	newbank->addUnusedSS(plane_id,(*uSS_iss).first,(*uSS_iss).second);
      } // end SS loop
    } // end unused planes loop
  } // end subregions loop

  int totroads = newbank->getNRoads();  

  if (road_warrior>=2) { // apply the cross-sector RW
    // re-order the roads to speed-up the following RW step
    newbank->sortRoads();

     if (debug_event) {
	cout << " => ";
	for (int isr=0;isr<nsubregions-1;++isr) 
	   cout << oldbanks[isr]->getNRoads() << '+';
	cout << oldbanks[nsubregions-1]->getNRoads() << '=' << totroads << endl;
	cout << "Apply Cross-Sector Road-Warrior: ";
     }
     int dbg_step = (totroads+9)/10; if(dbg_step==0) dbg_step=1;

     FTKRoadKDTree *kd_tree = newbank->buildKDTree();
     for (int imr=1;imr<totroads;++imr) { // loop over patterns to check the MJ roads
       if (debug_event && imr%dbg_step==0) {
	 cout << '#' << flush;
       }
       FTKRoad *chk = newbank->getRoad(imr);

       // skip full roads, only MJ roads can be deleted at this point
       if (chk->getNHits()==chk->getNPlanes()) continue;
       // already rejected roads
       if (chk->getRWRejected()) continue;

       // build the mask of majority planes
       unsigned int unconstrmask(0);
       // form the mask of the empty SS, that not constrains the search
       for (int ipl=0;ipl!=chk->getNPlanes();++ipl) {
	 if (!(chk->hasHitOnLayer(ipl))) 
	   unconstrmask |= (1<<ipl);
       }
       // ask for the list of the road sharing all the SSs, except
       // the ones in the majority planes of the reference road
       list<FTKRoadKDTree*> sims;
       /*int nsims = */ kd_tree->findSimilar(chk,unconstrmask,sims);

       // makes RW between similar road
       list<FTKRoadKDTree*>::iterator isim = sims.begin();
       for (;isim!=sims.end();++isim) {
	 // the refroad can be removed only by another in a greater position
	 // skip this otherwise
	 if ((*isim)->getPosition()>imr) continue;


	 // compare the two roads
	 /* when ref and chk are both N/N tracks the RW will fail,
	    in future this will be discarded but now is here as debug 
	    or to permit to test other algorithms */
	 FTKRoad *ref = (*isim)->getNode();
	 if (ref->getRWRejected()) continue;
	 if (informationMatch(*ref,*chk)) {
	   // mark chk as RW removed of type 2	  
	   chk->setRWRejected(2);
	   chk->setKillerRoad(ref);
	 }	 
       } // end loop over the similar patterns
     } // end loop over chech roads
     // delete the kd-tree
     delete kd_tree;
  } // end cross-sector code

  if (road_warrior>=3) {
     if (debug_event) {
	cout << endl;
	cout << "Apply Overlap Road-Warrior: ";
     }

     // if the roads are removed the total number of roads can be changed
     int dbg_step = (totroads+9)/10; if(dbg_step==0) dbg_step=1;
     // the block below applies a prototype for the overlap RW
     // the one eligibles to be used between banks
     for (int imr=0;imr<totroads;++imr) { // loop over reference patterns
	if (debug_event && imr%dbg_step==0) {
	   cout << '#' << flush;
	}

	FTKRoad *ref = newbank->getRoad(imr);

	// check if it is a good reference
	if (ref->getRWRejected()) continue; // rejected

	const PatternDefinition& sref = sectbank[ref->getRegion()][ref->getSubRegion()]->getPatternFast(ref->getSectorID());

	for (int jmr=imr+1;jmr<totroads;++jmr) { // loop over check patterns
	  
	   FTKRoad *chk = newbank->getRoad(jmr);

	   if (chk->getRWRejected()) continue; // rejected

	   const PatternDefinition& schk = sectbank[chk->getRegion()][chk->getSubRegion()]->getPatternFast(chk->getSectorID());
	  
	   bool res = FTKRoad::doStaticRoadWarrior(*ref,sref,*chk,schk);
	   if (res) {
	      // if the match is verified the chk road is marked as rejected
	      chk->setRWRejected(3);
	      chk->setKillerRoad(ref);
	   }
	    

	} // end loop over check patterns
     } // end loop over reference patterns
  } // end overlap-RW code

  // loop to clean the removed roads (for all kinds of road warrior)
  int nkilled = 0;
  for (int ir=0;ir!=totroads;++ir) {
    FTKRoad *cur_road = newbank->getRoad(ir);
    if (cur_road->getRWRejected()>0) {
      if (!keep_roads && road_warrior>0 ) newbank->removeRoad(ir);
      nkilled++;
    }
  }
  // clean the array of the roads in the bank to
  // remove the holes left by the rejected roads
  if (!keep_roads && road_warrior>0 ) newbank->optimize();

  newbank->naoSetNroadsRW(totroads-nkilled);
  
  if (debug_event) {
    cout << endl;
  }
  
}


bool informationMatch(const FTKRoad &leftroad, const FTKRoad &rightroad)
{
  // retrieve the number of the planes
  int nplanes = leftroad.getNPlanes();

  // if the "other" track has more hits than this RW can not be used
  if (leftroad.getNHits()<rightroad.getNHits())
    return false;

  unsigned int leftbitmask = leftroad.getBitMask();
  unsigned int rightbitmask = rightroad.getBitMask();

  if (!pmap) { // there isn't a valid pmap
    for (int i=0;i<nplanes;++i) { // loop over the planes
      /// logic from ftksim_code1.c, informationMatch(...)
      if ( leftroad.getSSID(i) != rightroad.getSSID(i) &&
	   (( ((leftbitmask&(1<<i))>>i)==1 &&
	      ((rightbitmask&(1<<i))>>i)==1 ) ||
	    ( ((leftbitmask&(1<<i))>>i)==0 &&
	      ((rightbitmask&(1<<i))>>i)==1 )) ) 
	return false;
    } // end loop over the planes
    
    return true;
  }
  else { // use the PMAP
    for(int i = 0; i < nplanes; ++i) { // loop to count the shared SS
      if(!pmap->isSCT(i)) { // Pixel plane
	/* pattern[][][] has the strips used by each pattern, if there is a
	   difference the two roads could be different.
	   The different is meaningful only if the two roads are empty in both
	   patterns or if the patt2 is empty while patt1 is a complete pattern*/
	if ( (leftroad.getSSID(i)!=rightroad.getSSID(i)) &&
	     (((leftbitmask&(1<<i))>0 && (rightbitmask&(1<<i))>0) 
	      ||
	      ((leftbitmask&(1<<i))==0 && (rightbitmask&(1<<i))>0))
	     ) {
	  /* The hitbit requirement is sufficient because because if patt1
	     has 1 empty SS patt2 has at least a different empty SS,
	     for this reason an empty SS in patt2 can be suspected to be
	     a ghost road while an empty SS in patt1 doesn-t */
	  //
	  /*
	    if((pattern[ibank][i][patt1]*(hitbit[ibank][i][patt1/8]&(1<<patt1%8))
	    - pattern[ibank][i][patt2])*(hitbit[ibank][i][patt2/8]&(1<<patt2%8))
	    //	 (hitbit[ibank][i][patt2]&1*hitbit[ibank][i][patt1]&1)) {
	    ) {*/
	  // here if the SS of the two patterns in this plane differs
#ifdef VERBOSE_DEBUG
	  printf("\t\t\tPIXEL %d and %d (%d) differs\n",pattern[ibank][i][patt1],
		 pattern[ibank][i][patt2],hitbit[ibank][i][patt2/8]&(1<<patt2%8));
#endif
	  return false;
	}
      } else { // SCT plane
	int ipair = pmap->getPlanePair(i);
	bool condition1 = (leftroad.getSSID(i)!=rightroad.getSSID(i)) &&
	  (((leftbitmask&(1<<i))>0 && 
	    (rightbitmask&(1<<i))>0) 
	   ||
	   ((leftbitmask&(1<<i))==0 && 
	    (rightbitmask&(1<<i))>0));

	// if SCT plane pairing is not used both condition are the same
	bool condition2 (condition1);
	if (i!=ipair) { // pairing is used
	  condition2 = (leftroad.getSSID(ipair)!=rightroad.getSSID(ipair)) &&
	    (((leftbitmask&(1<<i))>0 && 
	      (rightbitmask&(1<<i))>0) 
	     ||
	     ((leftbitmask&(1<<i))==0 && 
	      (rightbitmask&(1<<i))>0));
	  if (ipair==i+1) {
	    // the 1st SCT plane can be paired to the next,
	    // if it is the case the next step can be skipped
	    i += 1; // skip one step
	  }
	}

	if (  condition1 && condition2 ) {
	  // here if the SS of the two patterns in this plane differs
	  return false;
	}
      }
    } // end loop to count the shared SS
  } // end block if pmap is valid

#ifdef VERBOSE_DEBUG
  printf("\t\t\t%d and %d differ for %d super-strips\n",patt2,patt1,nsame);
#endif

  return true;
}
