/**
 **     @file    skim.cxx
 **
 **     @author  mark sutton
 **     @date    Mon 30 Jan 2012 18:43:21 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/



#include <stdlib.h>

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <regex>
#include <algorithm>


#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include "TrigInDetAnalysis/TIDAEvent.h"

#include "utils.h"

template<class T>
void remove_duplicates(std::vector<T>& vec) {
  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

std::string time_str() { 
  time_t _t;
  time(&_t);
  std::string s(ctime(&_t));
  return s.substr(0,s.find("\n"));
}

int usage(int e=0) { 
    std::cerr << "usage: skim <filename> [OPTIONS]" << std::endl;
    std::cerr << "\nremoves chains from the ntple\n";
    std::cerr << "\nOptions:\n";
    std::cerr << "\t-d | --delete\tremoves specified chains\n";
    std::cerr << "\t-o | --output\toutput filename (default tree.root)\n";
    std::cerr << "\t-r | --require\trequire that this chain has tracks\n";
    std::cerr << "\t-f | --force\tforce processing even if no chains specified\n";
    std::cerr << "\t     --pt value\tremove offline tracks pt < value\n";
    std::cerr << "\t     --roi  \tfilter offline tracks by roi phi\n";
    std::cerr << "\nIf option -d is not given, the specified chains are retained and all others are removed" << std::endl;
    std::cerr << "\nIf no chains are specifed simply all events with no L2 or EF chains are excluded" << std::endl;
    return e;
}

template<class T>
std::ostream& operator<<( std::ostream& s, const std::set<T>& _s ) { 
  typename std::set<T>::const_iterator sitr = _s.begin();
  s << "[  ";
  while ( sitr!=_s.end() ) s << (*sitr++) << "\t"; 
  s << " ]";
  return s;
}


template<class T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& _s ) { 
  typename std::vector<T>::const_iterator sitr = _s.begin();
  s << "[  ";
  while ( sitr!=_s.end() ) s << (*sitr++) << "\t"; 
  s << " ]";
  return s;
}



void copyReleaseInfo( TFile* finput, TFile* foutdir ) { 

  if ( finput && foutdir ) { 

    TTree* tree  = (TTree*)finput->Get("dataTree");    
    TTree* clone = tree->CloneTree();

    foutdir->cd();
    clone->Write("", TObject::kOverwrite);

    delete clone;

  }
  
}




int main(int argc, char** argv) { 
  
  if ( argc<1 ) return usage(-1);
  
  std::set<std::string>   require_chains;
  bool require  = false;
  bool deleting = false;

  std::string outfile = "tree.root";

  std::string infile="";

  bool adding_chains   = false;
  bool deleting_chains = false;

  bool force = false;

  double ptmin = 0;

  bool roi_filter = false;

  bool verbose = false;

  for ( int i=1 ; i<argc ; i++ ) { 

    std::string arg(argv[i]);

    if ( arg.find("-")!=0 ) { 
      if ( adding_chains || deleting_chains ) { 
	require_chains.insert(argv[i]); 
	continue;
      }
    }
    else { 
      adding_chains   = false;
      deleting_chains = false;
    }

    if      ( arg=="-h" || arg=="--help" )   return usage(0);
    else if ( arg=="-o" || arg=="--output" ) { 
      if ( (i+1)<argc )  outfile = argv[++i];
      else               return usage(-1);
    }
    else if ( arg=="-r" || arg=="--require" ) { 
      if ( deleting ) { 
	std::cerr << "cannot require and delete chains" << std::endl;
	return usage(-4);
      }
      adding_chains = true;
      require = true;
    }
    else if ( arg=="-d" || arg=="--delete" ) {
      if ( require ) { 
	std::cerr << "cannot require and delete chains" << std::endl;
	return usage(-3);
      }
      deleting_chains = true;
      deleting = true;
    }
    else if ( arg=="--pt" ) { 
      if ( (i+1)<argc )  ptmin = std::atof(argv[++i])*1000;
      else               return usage(-1);
      force = true;
    }
    else if ( arg=="-v" || arg=="--verbose" ) verbose = true;
    else if ( arg=="-f" || arg=="--force" )   force = true;
    else if (              arg=="--roi" )     { force=true; roi_filter = true; }
    else if ( infile=="" ) infile = arg;
    else { 
      std::cerr << "more than one file specified: " << arg << std::endl;
      return usage(-2);
    }
  }

  //  std::cout << "required chains " << require_chains << std::endl;

  if ( !force && require_chains.size()==0 ) { 
    std::cout << "no chains requested - not doing anything" << std::endl;
    return 0;
  }


  std::cout << "skim::start       " << time_str() << std::endl; 

  //  if ( require_chains.size()>0 ) require = true;

  if ( require  ) std::cout << "require chains " << require_chains << std::endl;
  if ( deleting ) std::cout << "delete  chains " << require_chains << std::endl;
  

  if ( infile=="" ) { 
    std::cerr << "no files specified" << std::endl;
    return usage(-1);
  }

  std::cout << "reading from file " << infile << std::endl;
  std::cout << "writing to file   " << outfile << std::endl;



  /// open output file
  TIDA::Event* track_ev = new TIDA::Event();
  TIDA::Event* h = track_ev;

  TFile fout( outfile.c_str(), "recreate");

  /// create the main event TTree ... 

  TTree *tree = new TTree("tree","tree");
  
  //  tree->Branch("Track","Int",&t,6400);
  tree->Branch("TIDA::Event", "TIDA::Event",&h,6400, 1);
    
  h->clear();



  /// event counters

  int ev_in  = 0;
  int ev_out = 0;
  
  {
    
    /// open inputfile
    TFile finput( infile.c_str() );
    if (!finput.IsOpen()) {
      std::cerr << "Error: could not open output file" << std::endl;
      exit(-1);
    }


    copyReleaseInfo( &finput, &fout );

    finput.cd();

    /// now the main event TTree    

    //   TChain* data = new TChain("tree");
    
    TTree* data = (TTree*)finput.Get("tree");

    TIDA::Event* track_iev = new TIDA::Event();
    
    data->SetBranchAddress("TIDA::Event",&track_iev);
    //    data->AddFile( argv[i] );
    

    
    for (unsigned int i=0; i<data->GetEntries() ; i++ ) {

      if ( verbose ) std::cout << "event " << i;

      track_iev->clear();
      track_ev->clear();

      data->GetEntry(i);      

      ev_in++;

      *track_ev = *track_iev;

      //      std::cout << *track_ev << std::endl;

      //      std::cout << "track_ev  names " << track_ev->chainnames()  << std::endl;
      //      std::cout << "track_iev names " << track_iev->chainnames() << std::endl;

      if ( verbose ) std::cout << "\tN chains " << track_ev->size() << " -> ";

      std::vector<TIDA::Chain>& chains = track_ev->chains();


      /// this bit of code will drop any event where the HLT hasn't passed
      bool skip = true;

      std::vector<TIDA::Chain>::iterator citr = chains.begin();
      for ( ; citr!=chains.end() ; citr++ ) {
	if ( citr->name().find("L2_")!=std::string::npos || 
	     citr->name().find("EF_")!=std::string::npos ||
	     citr->name().find("HLT_")!=std::string::npos ) skip = false;
      }

      if ( skip ) continue;


      /// now onto the business of deleting the chains ...

      { 

	/// delete the listed chains

	std::vector<std::string> chainnames = track_ev->chainnames();

	for ( size_t ic=chainnames.size() ; ic-- ; ) {
	  
	  bool matched = false;
	  for ( std::set<std::string>::iterator it=require_chains.begin() ; it!=require_chains.end() ; it++ ) { 
	    matched |= std::regex_match( chainnames[ic], std::regex(*it) );
	  }
	    
	  if ( ( require && !matched ) || ( deleting && matched ) ) track_ev->erase( chainnames[ic] );
	}
	        
	if ( verbose ) std::cout << track_ev->size() << std::endl;
      

	if ( roi_filter || ptmin>0 ) { 
	  
	  TIDA::Chain* offline = 0;

	  std::vector<std::string> chainnames = track_ev->chainnames();

	  /// get the offline chain

	  for ( size_t ic=chainnames.size() ; ic-- ; ) {
	    if ( chainnames[ic] == "Offline" ) {
	      offline = &(track_ev->chains()[ic]);
	      break;
	    }
	  }

	  if ( offline ) { 
	    // track_ev->addChain( "Offline" ); 
	    
	    std::vector<TIDA::Chain>& chains = track_ev->chains();
	    std::vector<TIDA::Chain>::iterator citr = chains.begin();
	    
	    std::vector<std::pair<double,double> > philims;
	    
	    for ( ; citr!=chains.end() ; citr++ ) {
	      if ( citr->name().find("HLT_")!=std::string::npos ) { 
		for ( size_t ir=0 ; ir<citr->size() ; ir++ ) {
		  TIDARoiDescriptor& roi = citr->rois()[ir].roi();
		  if ( roi.composite() ) { 
		    for ( size_t isub=0 ; isub<roi.size() ; isub++ ) { 
		      philims.push_back( std::pair<double,double>( roi[isub]->phiMinus(), roi[isub]->phiPlus() ) ); 
		    }
		  }
		  else philims.push_back( std::pair<double,double>( roi.phiMinus(), roi.phiPlus() ) ); 
		}
	      }
	    }

	    remove_duplicates( philims );

	    for ( size_t iroi=0 ; iroi<offline->size() ; iroi++ ) {

	      std::vector<TIDA::Track>& tracks = offline->rois()[iroi].tracks();
	      
	      for ( std::vector<TIDA::Track>::iterator it=tracks.begin() ; it<tracks.end() ; ) {
		bool inc = true;
		if ( ptmin>0 ) { 
		  if ( std::fabs(it->pT())<ptmin ) { inc=false; tracks.erase( it ); }
		}
		if ( inc && roi_filter ) { 
		  bool remove_track = true;
		  for ( size_t isub=0 ; isub<philims.size() ; isub++ ) { 

		    if ( philims[isub].first < philims[isub].second ) { 
		      if ( it->phi()>=philims[isub].first && it->phi()<=philims[isub].second ) { 
			remove_track = false; 
			break;
		      }
		    }
		    else  {
		      if ( it->phi()>=philims[isub].first || it->phi()<=philims[isub].second ) { 
			remove_track = false; 
			break;
		      }
		    }
		  }
		  if ( remove_track ) { inc=false; tracks.erase( it ); }
		}
		if ( inc ) it++;
	      }

	    }

	  }


	}
	
	//      std::cout << "writing event " << track_ev->event_number() << " <<<<<<<<<<<<<<<<<<<<" << std::endl; 
	tree->Fill();
	ev_out++;
	
	
#if 0
	for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) {
	  if ( chains[ic].name()=="Offline" ) {
	    const std::vector<TIDA::Track>& tracks = chains[ic].rois()[0].tracks();
	    
	    for ( unsigned it=0 ; it<tracks.size() ; it++ ) {
	      h->Fill( tracks[it].pT()*0.001 );
	      itracks++;
	      
	    }
	    break;
	  }
	}
#endif     
	
      }
    }
    
    finput.Close();
    
  }

  std::cout << "skim::done        " << time_str() << std::endl; 

  std::cout << "skim::events in:  " << ev_in  << std::endl;
  std::cout << "skim::events out: " << ev_out << std::endl;
  
  fout.Write();
  fout.Close();
  
  return 0;
}



