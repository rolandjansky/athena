/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/TrackFitter.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTKSim/ftkdefs.h"
#include "common_fcn.h"

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <list>
using namespace std;

unsigned int nbanks(0);
unsigned int nsubs(1);

unsigned int ncoords(0);

unsigned int nfiles(0);

unsigned int total_nevts(0);

int input_tree_merged(0); // set to 1 if this is a global merge of 8 per-region track_mergers

int HW_level(1); // hit-warrior level

unsigned int HW_ndiff(3); // maximum number of different points

float *HW_dev(0); //[ncoords] tolerances for the HW

int keep_rejected(0); // >0 keep rejected roads (1 HW rej, 2 bad quality)

unsigned int nfits_rej(0);

// lists with path of the files to merge for each region
vector<string> **pathlist(0);

// output basedir
string outdir("");

// function to parse the input
int read_commands();


// function that merges N input stream into 1, applying HW rejection
void merge_tracks(FTKTrackStream *&, FTKTrackStream ***);
std::list<FTKTrack>::iterator removeTrack(std::list<FTKTrack>&, std::list<FTKTrack>::iterator, FTKTrack&, const FTKTrack&,bool isnew=false);

/** this program reads n input files, containing FTK tracks, and
    merger the tracks in the different banks in a single bank
    collectings all the tracks */
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

  // global number of events
  unsigned int tot_evt(0);

  // allocate memory
  TFile ***reg_file = new TFile**[nbanks];
  TTree ***reg_tree = new TTree**[nbanks];
  FTKTrackStream ***reg_bank = new FTKTrackStream**[nbanks];
  for (unsigned int ib=0;ib!=nbanks;++ib) { // region loop
    reg_file[ib] = new TFile*[nsubs];
    reg_tree[ib] = new TTree*[nsubs];
    reg_bank[ib] = new FTKTrackStream*[nsubs];
  }

  // loop over the files
  cout << "Loop over # " << nfiles << " files" << endl;
  for (unsigned int ifile=0;ifile<nfiles;++ifile) {
    cout << "Open original trees" << endl;
    
   // reset all pointers to null
    for (unsigned int ib=0;ib!=nbanks;++ib) { // region loop
      for (unsigned int is=0;is!=nsubs;++is) { // subregion loop
	reg_file[ib][is]=0;
	reg_tree[ib][is]=0;
	reg_bank[ib][is]=0;
      }
    }

    // the number of event will be set by the first valid region
    unsigned int nevents(0);
    // this will become equal the filename of the first valid region
    string merged_fname("merged_file.root");

    // loop to open original TTree
    for (unsigned int ib=0;ib!=nbanks;++ib) { // region loop
      for (unsigned int is=0;is!=nsubs;++is) { // region loop
	if (pathlist[ib][is].empty()) continue;
	cout << "Opening: " << pathlist[ib][is][ifile] << endl;
	reg_file[ib][is] = TFile::Open(pathlist[ib][is][ifile].c_str());
	reg_tree[ib][is] = (TTree*) reg_file[ib][is]->Get("ftkdata");
	if (!reg_tree[ib][is]) {
	  cerr << "*** Error: TTree in file \"" << pathlist[ib][is][ifile] << "\" not found" << endl;
	  return -1;
	}
	reg_bank[ib][is] = new FTKTrackStream();
	if(input_tree_merged) // if merging output of track_merger
	  reg_tree[ib][is]->SetBranchAddress("FTKBankMerged",&reg_bank[ib][is]);
	else {                 // if merging output of track_fitter
	  reg_tree[ib][is]->SetBranchAddress(Form("FTKBank%d.",ib),&reg_bank[ib][is]);
	}
	// assume the standard number of events from the first TTree
	if (nevents==0) {
	  nevents = reg_tree[ib][is]->GetEntries();
	  merged_fname = gSystem->BaseName(pathlist[ib][is][ifile].c_str());
	}

      } // end subregion loop
    } // end region loop

    // create the merged file
    string ofname = Form("%s/%s",outdir.c_str(),merged_fname.c_str());
    TFile *ofile = TFile::Open(ofname.c_str(),"recreate");
    
    // create the TTree and the branches
    TTree *otree = new TTree("ftkdata","FTK Simulation Results (merged)");
    // recent bank object
    FTKTrackStream *merged_bank = new FTKTrackStream;
    otree->Branch("FTKBankMerged",&merged_bank,TREE_ROAD_BUFSIZE);

    unsigned int iev_step = nevents>500 ? nevents/100 : 1;
    cout << "Begin loop over # " << nevents << " events" << endl;
    for (unsigned int iev=0;iev!=nevents&&tot_evt!=total_nevts;++iev,++tot_evt) { // event loop

      if (iev%iev_step==0) {
	cout << "Event " << iev << "  global event " << tot_evt << endl;
      }

      if (iev%(iev_step*10)==0)
	ftkset.usageStat();

      for (unsigned int ib=0;ib!=nbanks;++ib) {
	for (unsigned int is=0;is!=nsubs;++is) {
	  if(!reg_tree[ib][is]) continue;
	  reg_tree[ib][is]->GetEntry(iev);
	}
      }
      
      // clear clones for this bank
      merged_bank->clear();
	
      // update roads[ib] summing, after the RW, the original roads
      merge_tracks(merged_bank,reg_bank);
      
      // fill the completed results
      otree->Fill();
    } // end event loop

    // write, close, and destroy the file
    ofile->Write();
    ofile->Close();
    delete ofile;

    // close input filesb
    set<TFile*> already_deleted;
    for( unsigned int ib=0;ib!=nbanks;++ib ) { // region loop
      for( unsigned int is=0;is!=nsubs;++is ) { // subregion loop
	if( !(reg_file[ib][is]) ) { continue; }
	if( already_deleted.find( reg_file[ib][is] ) != already_deleted.end() ) {
	  reg_file[ib][is] = 0;
	  continue; 
	}
	already_deleted.insert( reg_file[ib][is] );
	delete reg_file[ib][is];
	reg_file[ib][is] = 0;
      }
    }

    
    // clean up other used memory;
    if( merged_bank ) { delete merged_bank; merged_bank = 0; }
    for (unsigned int ib=0;ib!=nbanks;++ib) { // region loop
      for (unsigned int is=0;is!=nsubs;++is) { // subregion loop
	if( reg_bank[ib][is] ) { delete reg_bank[ib][is]; reg_bank[ib][is] = 0; }
      }
    }
    // if( otree ) {
    //   delete otree; 
    //   otree = 0;
    // }
    
  }


  return 0;
}


/** this function loads the HW distance used to decide if 2 coordinates
    represent a single real hit */
void loadHWConf(const char *fname)
{
  ifstream fin(fname);
  if (!fin) {
    FTKSetup::PrintMessage(ftk::sevr,"Error loading HW configuration");
  }

  string line;

  // first line represents version, not used now
  getline(fin,line);
  
  // loop until an N in the first position is found,
  // this should change in the future, as in other map files
  while(getline(fin,line)) {
    if (line[0]=='N') break;
  }

  // start the real read of the file
  char flag;
  unsigned int nvals;
  int pos;
  float fval;

  fin >> flag >> nvals;
  if (flag!='N') 
    FTKSetup::PrintMessage(ftk::sevr,"Error parsing HW configuration");
  else if (nvals!=ncoords) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Error parsing HW configuration, wrong number of values %d -> %d",nvals,ncoords);
  }

  for (unsigned int i=0;i<nvals;++i) {
    fin >> pos >> fval;
    HW_dev[pos] = fval;
  }
}


/** parse the input files */
int read_commands()
{
    string line;
  while (replace_getline(cin,line)) { // loop over the configuration file
    istringstream sline(line);
    cout << "PARSE> " << line << endl;
    // skip empty lines and comments
    if (!line.size()) continue;
    else if (line[0]=='#') continue;

    string key;
    sline >> key;

    if (key=="NBANKS") {
      // set the number of the banks
      sline >> nbanks;
      pathlist = new vector<string>*[nbanks];
    }
    else if (key=="NSUBREGIONS") {
      sline >> nsubs;
      for (unsigned int i=0;i<nbanks;++i) {
	pathlist[i] = new vector<string>[nsubs];
      }
    }
    else if (key=="INPUT_TREE_MERGED") {
      sline >> input_tree_merged;
    }
    else if (key=="NCOORDS") {
      sline >> ncoords;
      // size the HW map
      HW_dev = new float[ncoords];

      for (unsigned int i=0;i!=ncoords;++i) {
        HW_dev[i] = 0.f;
      }
    }
    else if (key=="NEVENTS") {
      sline >> total_nevts;
    }
    else if (key=="NFILES") {
      // setup the number of files to load for each bank
      sline >> nfiles;
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

      const int ir_offset = doRegLoop ? 1 : nbanks;
      const int is_offset = doSubLoop ? 1 : nsubs;
      for (unsigned int ir=regid;ir!=nbanks;ir+=ir_offset) { // region loop
	for (unsigned int is=subid;is!=nsubs;is+=is_offset) { // sub-region loop
	  cin.seekg(fpos);

	  for (unsigned int i=0;i!=nfiles;++i) {
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
	      
	      pathlist[ir][is].push_back(path);
	    }
	  }
	  if (!doSubLoop) break;
	} // end subregion loop
	if (!doRegLoop) break;
      } // end region loop
      
    }
    else if (key=="OUT_DIR") {
      string sval;
      sline >> sval;
      outdir = sval;
    }
    else if (key=="HITWARRIOR") {
      sline >> HW_level;
    }
    else if (key=="HW_NDIFF") {
      sline >> HW_ndiff;
    }
    else if (key=="HWSDEV_FILE") {
      string sval;
      sline >> sval;
      loadHWConf(sval.c_str());
    }
    else if (key=="KEEP_REJECTED") {
      sline >> keep_rejected;
    }
    
    else {
      cout << "*** unknown directive" << endl;
    }
  } // end loop over the configuration file


  return 0;
}



/** this function merges the tracks coming from the banks produced
    fitting the roads in a single banks, the output will have tracks
    from roads found in all the detectors*/
void merge_tracks(FTKTrackStream* &merged_tracks, FTKTrackStream ***reg_tracks)
{
  list<FTKTrack> tracks_list;
  
  // reset rejected fits counter
  nfits_rej = 0;

  // copy run and event number to the output, and check that the
  // banks are in sync.
  unsigned long bank_run = 0ul;
  unsigned long bank_event = 0ul;
  for (unsigned int ib=0; ib!=nbanks;++ib) { // banks loop
    for (unsigned int is=0; is!=nsubs;++is) { // subregions loop
      if(!reg_tracks[ib][is]) continue;
      unsigned long this_run = reg_tracks[ib][is]->runNumber();
      unsigned long this_event = reg_tracks[ib][is]->eventNumber();
      //if( this_run==0 && this_event==0 ) { continue; }
      bank_run = this_run;
      bank_event = this_event;
      break;
    }
  }
  merged_tracks->setRunNumber( bank_run );
  merged_tracks->setEventNumber( bank_event );
  //if( bank_run!=0 || bank_event!=0 ) {
  bool event_sync = true;
  for(unsigned int ib=0; ib!=nbanks; ++ib) {
    for (unsigned int is=0; is!=nsubs;++is) {
      if(!reg_tracks[ib][is]) continue;
      unsigned long this_run = reg_tracks[ib][is]->runNumber();
      unsigned long this_event = reg_tracks[ib][is]->eventNumber();
      if( this_run != bank_run || this_event != bank_event ) {
	event_sync = false;
	break;
      }
    }
  }
  if( !event_sync ) {
    std::cout << "BANKS ARE OUT OF SYNC!!!" << std::endl;
    std::cout << "Bank\t\tSubregion\t\tRun\t\tEvent!" << std::endl;
    for(unsigned int ib=0; ib!=nbanks; ++ib) {
      for(unsigned int is=0; is!=nsubs; ++is) {
	std::cout << ib << "\t\t" << is << "\t\t" << reg_tracks[ib][is]->runNumber() << "\t\t" << reg_tracks[ib][is]->eventNumber() << std::endl;
      }
    }
    return;
  }
    //}


  for (unsigned int ib=0;ib!=nbanks;++ib) { // bank loop
    for (unsigned int is=0; is!=nsubs;++is) { // subregions loop
      if(!reg_tracks[ib][is]) continue;
      
      unsigned int ntracks = reg_tracks[ib][is]->getNTracks();
      
      merged_tracks->addNCombs(reg_tracks[ib][is]->getNCombs());
      merged_tracks->addNFits(reg_tracks[ib][is]->getNFits());
      merged_tracks->addNFitsMajority(reg_tracks[ib][is]->getNFitsMajority());
      merged_tracks->addNFitsRecovery(reg_tracks[ib][is]->getNFitsRecovery());
      merged_tracks->addNAddFitsRecovery(reg_tracks[ib][is]->getNAddFitsRecovery());
      merged_tracks->addNFitsBad(reg_tracks[ib][is]->getNFitsBad());
      merged_tracks->addNFitsHWRejected(reg_tracks[ib][is]->getNFitsHWRejected());
      merged_tracks->addNFitsBadMajority(reg_tracks[ib][is]->getNFitsBadMajority());
      merged_tracks->addNFitsHWRejectedMajority(reg_tracks[ib][is]->getNFitsHWRejectedMajority());


      unsigned int ntracksI = reg_tracks[ib][is]->getNTracksI();
      merged_tracks->addNCombsI(reg_tracks[ib][is]->getNCombsI());
      merged_tracks->addNFitsI(reg_tracks[ib][is]->getNFitsI());
      merged_tracks->addNFitsMajorityI(reg_tracks[ib][is]->getNFitsMajorityI());
      merged_tracks->addNFitsRecoveryI(reg_tracks[ib][is]->getNFitsRecoveryI());
      merged_tracks->addNAddFitsRecoveryI(reg_tracks[ib][is]->getNAddFitsRecoveryI());
      merged_tracks->addNFitsBadI(reg_tracks[ib][is]->getNFitsBadI());
      merged_tracks->addNFitsHWRejectedI(reg_tracks[ib][is]->getNFitsHWRejectedI());
      merged_tracks->addNFitsBadMajorityI(reg_tracks[ib][is]->getNFitsBadMajorityI());
      merged_tracks->addNFitsHWRejectedMajorityI(reg_tracks[ib][is]->getNFitsHWRejectedMajorityI());
      
      for (unsigned int itr=0;itr!=ntracks;++itr) { // track loop
	// get the track from the bank
	FTKTrack &newtrack = *(reg_tracks[ib][is]->getTrack(itr));
	
	
	// remains 0 if the track has to be added
	// -1 means is worse than an old track (duplicated)
	int accepted(0);
	
	list<FTKTrack>::iterator itrack = tracks_list.begin();
	// Disable hitwarrior according the flags
	if (HW_level==0 || newtrack.getHWRejected()/100!=0)
	  itrack=tracks_list.end();
	
	for (;itrack!=tracks_list.end();++itrack) { // loop over tracks of this bank
	  // reference to an old track
	  FTKTrack &track_old = *itrack;
	  if (track_old.getHWRejected()%10 !=0 || track_old.getHWRejected()/100!=0) 
	    // skip HW rejected tracks and tracks with bad chi2,
	    // tracks in removed roads are used, but if a match is found 
	    // will be always rejected or marked as removed, if compared
	    // to a combination in a good road
	    continue; 
	  
	  int HWres = newtrack.HWChoice(track_old,HW_dev,HW_ndiff,HW_level);
	  
	  
	  if (HWres==-1) {
	    accepted = -1;
	    // passing the end of the list means remove current
	    removeTrack(tracks_list,tracks_list.end(),newtrack,track_old,true);
	  }
	  else if (HWres==1) {
	    // return the new current position, it changes if the
	    // rejected track is removed from the list
	    itrack = removeTrack(tracks_list,itrack,track_old,newtrack);
	  }
	  
	} // end loop over tracks of this bank

     
	// create a copy of the track into the merged list
	if (accepted==0) {
	  tracks_list.push_back(newtrack);
	}
	else if (keep_rejected>0) {
	  tracks_list.push_back(newtrack);	
	}
      } // end track loop



      for (unsigned int itrI=0;itrI!=ntracksI;++itrI) { // intermediate track loop
	// get the track from the bank
	FTKTrack &newtrack = *(reg_tracks[ib][is]->getTrackI(itrI));
	
	// the intermediate tracks are not filtered by HW
	merged_tracks->addTrackI(newtrack);
      } // end intermediate track loop

    } // end subregion loop
  } // end bank loop
  
  // synchronize the rejected counter
  merged_tracks->addNFitsHWRejected(nfits_rej);
  
  // push the tracks into the output stream
  list<FTKTrack>::iterator itrack = tracks_list.begin();
  for (;itrack!=tracks_list.end();itrack = tracks_list.erase(itrack)) { // loop over merged tracks
    merged_tracks->addTrack(*itrack);
  } // end loop over merged tracks
}


/** rejected, or mark as rejected, the track in the list
    against  the second track */
list<FTKTrack>::iterator removeTrack(list<FTKTrack> &tracks_list, list<FTKTrack>::iterator itrack,
				     FTKTrack &rejtrk, const FTKTrack &killer, bool rejnew)
{
  
  if (!rejnew) { // is rejecting a track in the list
    if (!keep_rejected) { // remove from the list the rejected
      // save the position before this
      list<FTKTrack>::iterator before = itrack;
      // move the to the previous track
      --before;

      // remove the old track from the list
      tracks_list.erase(itrack);

      // assign the itrack to the previous position before, to not invalidate the loop
      itrack = before;
    }
    else if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      nfits_rej += 1;
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());    

      nfits_rej += 1;
    }
  }

  return itrack;
}
