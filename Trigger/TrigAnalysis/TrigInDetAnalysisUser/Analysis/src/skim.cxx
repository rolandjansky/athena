//
//   @file    reader.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: reader.cxx, v0.0   Mon 30 Jan 2012 18:43:21 CET sutt $



#include <stdlib.h>

#include <iostream>
#include <vector>
#include <set>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include "TrigInDetAnalysis/TrackEvent.h"

#include "utils.h"




int usage(int e=0) { 
    std::cerr << "usage: skim <filename>  -f <outfile> [-d|--delete] <chain1> <chain2> ... -f <outfile> -r <chain>" << std::endl;
    std::cerr << "\nremoves chains from the ntple\n";
    std::cerr << "\nOptions:\n";
    std::cerr << "\t-d | --delete\tremoves specified chains\n";
    std::cerr << "\t-f | --file\toutput filename (default tree.root)\n";
    std::cerr << "\t-r | --require\trequire that this chain has tracks\n";
    std::cerr << "\nIf option -d is not given, the specified chains are retained and all others are removed" << std::endl;
    std::cerr << "\nIf no chains are specifed simply all events with no L2 or EF chains are excluded" << std::endl;
    return e;
}

template<class T>
std::ostream& operator<<( std::ostream& s, const std::set<T>& _s ) { 
  typename std::set<T>::const_iterator sitr = _s.begin();
  s << "[ ";
  while ( sitr!=_s.end() ) s << "\t" << (*sitr++); 
  s << " ]";
  return s;
}



int main(int argc, char** argv) { 
  
  if ( argc<1 ) return usage(-1);
  
  std::set<std::string>   user_chains;
  bool                  delete_chains = false;


  std::set<std::string>   require_chains;
  bool                    require = false;


  std::string outfile = "tree.root";

  std::string infile="";

  for ( int i=1 ; i<argc ; i++ ) { 

    std::string arg(argv[i]);

    if      ( arg=="-h" || arg=="--help" )   return usage(0);
    else if ( arg=="-d" || arg=="--delete" ) delete_chains = true;
    else if ( arg=="-f" ) { 
      if ( (i+1)<argc )  outfile = argv[++i];
      else               return usage(-1);
    }
    else if ( arg=="-r" || arg=="--require" ) { 
      if ( (i+1)<argc )  require_chains.insert(argv[++i]);
      else               return usage(-1);
    }
    else if ( infile=="" ) infile = arg;
    else                  user_chains.insert( arg );

  }

  if ( require_chains.size()>0 ) require = true;

  if ( delete_chains ) { 
    std::cout << "delete chains " << user_chains << std::endl;
  }
  else { 
    std::cout << "retain chains " << user_chains << std::endl;
  }

  if ( require ) { 
    std::cout << "require chains " << require_chains << std::endl;
  }

  if ( infile=="" ) return usage(-1);

  std::cout << "reading from file " << infile << std::endl;
  std::cout << "writing to file   " << outfile << std::endl;
     
  /// open output file
  TrackEvent* track_ev = new TrackEvent();
  TrackEvent* h = track_ev;

  TFile fout( outfile.c_str(), "recreate");
  TTree *tree = new TTree("tree","tree");
  
  //  tree->Branch("Track","Int",&t,6400);
  tree->Branch("TrackEvent", "TrackEvent",&h,6400, 1);
    
  h->clear();
      
  
  {
    
    /// open inputfile
    TFile finput( infile.c_str() );
    if (!finput.IsOpen()) {
      std::cerr << "Error: could not open output file" << std::endl;
      exit(-1);
    }
    
    //   TChain* data = new TChain("tree");
    
    TTree* data = (TTree*)finput.Get("tree");
    
    data->SetBranchAddress("TrackEvent",&track_ev);
    //    data->AddFile( argv[i] );
    
    
    for (unsigned int i=0; i<data->GetEntries() ; i++ ) {
      data->GetEntry(i);
      
      //    std::cout << *track_ev << std::endl;

      std::vector<TrackChain>& chains = track_ev->chains();

      bool skip = true;

      std::vector<TrackChain>::iterator citr = chains.begin();
      for ( ; citr!=chains.end() ; citr++ ) {
	if ( citr->name().find("L2_")!=std::string::npos || citr->name().find("EF_")!=std::string::npos ) skip = false;
      }

      if ( skip ) continue;

      
      if ( require ) { 

	bool found = false;

	/// now check that any required chain is satidfied
	citr = chains.begin();
	for ( ; citr!=chains.end() ; citr++ ) {

	  //	  std::cout << "chain " << citr->name() << "  " <<  (require_chains.find(citr->name())!=require_chains.end()) << std::endl;

	  if ( require_chains.find(citr->name())!=require_chains.end() ) { 

	    //	    std::cout << "found chain " << citr->name() << std::endl;

	    std::vector<TrackRoi>& rois = citr->rois();

	    for ( unsigned ir=0 ; ir<rois.size() ; ir++ ) if ( rois[ir].tracks().size()>0 ) found = true;
	    
	  }
	}
	
	if ( found==false ) continue;

      }


      bool deleted_chains = false;

      if ( delete_chains ) { 
	/// delete the listed chains
	std::vector<TrackChain>::iterator citr = chains.begin();
	for ( ; citr!=chains.end() ; ) {
	  if ( user_chains.find(citr->name())!=user_chains.end() ) { 
	    chains.erase( citr );
	    deleted_chains = true;
	  }
	  else citr++; /// only increment if you *haven't* erased a chain
	}
      }
      else { 
	/// delete evenrything *except* the listed chains
	if ( user_chains.size() > 0 ) { 
	  std::vector<TrackChain>::iterator citr = chains.begin();
	  for ( ; citr!=chains.end() ; ) {
	    if ( user_chains.find(citr->name())==user_chains.end() ) { 
	      chains.erase( citr );
	      deleted_chains = true;
	    }
	    else citr++; /// only increment if you *haven't* erased a chain
	  }
	}
      }

      if ( deleted_chains ) {
	/// now, if we have deleted any chains, check again that there are still some 
	/// trigger chains left (no point saving anything if we have deleted them all)  
	skip = true;
	
	citr = chains.begin();
	for ( ; citr!=chains.end() ; citr++ ) {
	  if ( citr->name().find("L2_")!=std::string::npos || citr->name().find("EF_")!=std::string::npos ) skip = false;
	}
	
	if ( skip ) continue;
      }

      //      std::cout << "writing event " << track_ev->event_number() << " <<<<<<<<<<<<<<<<<<<<" << std::endl; 
      tree->Fill();


#if 0
      for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) {
	if ( chains[ic].name()=="Offline" ) {
	  const std::vector<TrigInDetAnalysis::Track>& tracks = chains[ic].rois()[0].tracks();

	  for ( unsigned it=0 ; it<tracks.size() ; it++ ) {
	    h->Fill( tracks[it].pT()*0.001 );
	    itracks++;

	  }
	  break;
	}
      }
#endif     
 
    }
    
    finput.Close();
    
  }
  
  fout.Write();
  fout.Close();
  
  return 0;
}



