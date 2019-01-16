/**
 **     @file    refit.cxx
 **
 **     @brief  navigates through the directory structure of a file copying 
 **             the histograms or recalculating efficiencies, or refitting 
 **             Resplots if it finds any
 **
 **     @author  mark sutton
 **     @date    Thu  5 Sep 2013 20:32:47 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>

#include "TKey.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TClass.h"

#include "simpletimer.h"


#include "dataset.h"

#include "TrigInDetAnalysis/Efficiency.h"
#include "Resplot.h"

// #include "Resplot.h"
// #include "TrigInDetAnalysis/Efficiency.h"

/// file names and file pointers
std::vector<TFile*>      fptr;


/// glabal timer - how long have I taken so far?
struct timeval global_timer;


/// tag string for intendation
std::string depth = ""; 

/// counter of the current depth
int ir=0;






bool contains( const std::string& s, const std::string& regex ) { 
  return s.find( regex )!=std::string::npos;
}


bool handleEfficiency( TDirectory* tnd ) { 
 
  TList* list = tnd->GetListOfKeys();
  //  std::cout << "list size " << list->GetSize() << std::endl;
  
  for ( int i=0 ; i<list->GetSize() ; i++ ) {
    
    std::string name( list->At(i)->GetName() );

    TKey* tobj = (TKey*)list->At(i);

    TH1F* eff = 0;

    if ( contains( tobj->GetClassName(), "TH1" ) ) { 

      if (    contains( name, "_eff" ) && 
	      ( !contains( name, "_eff_n" )  && !contains( name, "_eff_d" ) ) ) { 

	eff = (TH1F*)tobj->ReadObj();
	
	std::string dname = name+"_d";
	std::string nname = name+"_n";
	
	TH1F* num = 0;
	TH1F* den = 0;
	
	for ( int j=0 ; j<list->GetSize() ; j++ ) {
	  
	  std::string _name( list->At(j)->GetName() );
	  
	  if ( _name==dname ) den = (TH1F*)((TKey*)list->At(j))->ReadObj();
	  if ( _name==nname ) num = (TH1F*)((TKey*)list->At(j))->ReadObj();
	  
	  if ( den && num ) { 
	    /// hooray !!!
	    std::cout << "Efficiency " << eff->GetName() << std::endl;
	    
	    std::string title = eff->GetTitle();

	    delete eff;

	    Efficiency e( num, den, name );
	    eff = e.Hist();
	    eff->SetName( name.c_str() );
	    eff->SetTitle( title.c_str() );

	    eff->Write("",TObject::kOverwrite);

	    break;
	  }
	}
	
      }
      
    }

  }

  return true; 
}

 


bool isResplot( TDirectory* tnd ) { 

  if ( tnd==0 ) return false; 

  static const std::string token[6] = {    "1d",   "2d",  "mean", "sigma", "chi2", "fractional uncertainty" };
  bool                     found[6] = {  false,  false,   false,   false,  false,  false };
  int count = 0;

  //  tnd->pwd();
  //  tnd->ls();
  //  std::cout << "\tnkeys " << tnd->GetNkeys() << std::endl; 
  TList* list = tnd->GetListOfKeys();
  //  std::cout << "list size " << list->GetSize() << std::endl;
  
  for ( int i=0 ; i<list->GetSize() ; i++ ) {

    std::string name( list->At( i )->GetName() );

    //    std::cout << "name " << name << std::endl;

    for ( int j=0 ; j<6 ; j++ ) { 
      if ( !found[j] ) { 
	if ( name == token[j] ) { 
	  count++;
	  found[j] = true;
	  break;
	}
      }
    }
    
  }
  
  std::cout << "Resplot count " << count << " " << tnd->GetName() << std::endl;
  
  if ( count == 6 ) return true;
  else              return false;
}





/// recursive directory search for TH1 and TH2

void search(TDirectory* td=0, const std::string& s="") { 

  ir++;

  /// don;t go more than 10 directories deep

  if ( ir>10 ) exit(0);



  TDirectory* here = gDirectory;

  //  gDirectory->pwd();

  std::string ns = s;

  if ( ns!="" ) ns += "/"; 

  if ( td )  { 
    gDirectory->cd(td->GetName());

    ns +=  td->GetName();

    std::cout << "\n" << depth << "Directory " << ns; // << std::endl;
  }

  std::string savedepth = depth;
  depth += "\t";

  //  std::cout << ir << " " << ns << "TDirectory " << gDirectory->GetPath() << " " << gDirectory->GetName() << std::endl;

  TList* tl  = gDirectory->GetListOfKeys();

  struct timeval tv = simpletimer_start();

  //  std::cout << "\ttl " << tl << std::endl;

  handleEfficiency( gDirectory );

  for ( int i=tl->GetSize() ; i-- ; ) { 

    TKey* tobj = (TKey*)tl->At(i);

    if ( tobj==0 ) continue;

    //    std::cout << "tobj " << tobj;
    //    if ( tobj ) std::cout << " : \t" << tobj->GetName();
    //    std::cout << std::endl; 
    
    if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
      //      std::cout << ns << "Directory " << tobj->GetName() << std::endl;
 
      TDirectory* tnd = (TDirectory*)tobj->ReadObj();
      
      if ( tnd ) { 
	bool res = isResplot( tnd );

	if ( res ) { 

	  tnd->ls();
	  tnd->pwd();

	  std::cout << "Resplot refitting " << tnd->GetName() << std::endl; 
	  Resplot r( tnd->GetName() );

	  std::cout << "deleting ..." << std::endl;
	  gDirectory->Delete( (std::string(tnd->GetName())+";1").c_str() );

	  std::cout << "refitting ..." << std::endl;
	  r.Refit( Resplot::FitNull95 );


	  std::cout << "writing ..." << std::endl;

	  r.Write();
	}
	else search( tnd, ns );
    
      }

    }
    //    else { 
    //      bool status = false;
    // 
    //      std::string objname = ns;
    //      if ( objname!="" ) objname += std::string("/") + tobj->GetName();
    //  
    //      if ( std::string(tobj->GetClassName()).find("TH1")!=std::string::npos )  std::cout << "objname " << objname.c_str() << std::endl;
    //
    //    }

  }

  double _t = simpletimer_stop(tv);
  
  double global_time = simpletimer_stop(global_timer);

  
  std::cout << "\tprocessed directory in " << _t*0.001 << " s  from " << global_time*0.001 << " s";


  depth = savedepth;

  ir--;

  here->cd();

}




#include <cstdlib>



bool file_exists(const std::string& s) {   
  if ( FILE* testfile=fopen(s.c_str(),"r") ) { 
    fclose(testfile);
    return true;
  }
  return false;
}




void cost( const std::string& inputfile, const std::string& outputfile ) { 

  /// no point doing anything if there is only 1 input file
  /// because there would be nothing to add to it

  std::cout << "processing" << std::endl;

  /// don't over write esisting files
  if ( file_exists(outputfile) ) { 
    std::cerr << "output file " << outputfile << " already exists" << std::endl;
    return;
  }

  std::cout << "copying file to " << outputfile << std::endl;

  /// copy the first file so we add to this copy
  system( ("cp "+inputfile+" "+outputfile).c_str() );


  /// don't bother to open the first file - we've copied 
  /// it to the output and will update the contents

  std::cout << "opening root files" << std::endl;


  std::cout << "opening " << inputfile << std::endl;

  fptr.resize(2);

  /// open the output file
  fptr[0] = new TFile( outputfile.c_str(), "update" );

  if ( fptr[0]==0 ) { 
    std::cerr << "cannot open " << outputfile.c_str() << std::endl;
    return;
  }
  
  fptr[0]->cd();
  
  TDirectory* here = gDirectory;

  global_timer = simpletimer_start();

  /// recursively look through the directory
  /// structure   
  search();
  
  here->cd();
  
  std::cout << "closing the files" << std::endl; 

  /// don't need to write the histogram again - writing them as we go along
  //  f0->Write();
  fptr[0]->Close();

}




int usage(std::ostream& s, int argc, char** argv) { 
  if ( argc>0 ) { 
    s << "Usage: " << argv[0] << " [OPTIONS] -o output.root  input.root \n\n";
    s << "    -o filename   \toutput filename (required)\n";
    s << "    -h, --help    \tdisplay this help\n";
    s << std::endl;
  } else { 
    s << "refit:usage() no arguments!\n";
  }
  return 0;
}




int main(int argc, char** argv) { 

  //  if ( argc<3 ) usage( std::cerr << "not enough command options", argc, argv );
  if ( argc<3 ) return usage( std::cerr, argc, argv );


  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" )  return usage( std::cout, argc, argv ); 
    //    if ( std::string(argv[i])=="-v" || std::string(argv[i])=="--version" ) {
    //      std::cout << argv[0] << " APPLgrid version " << PACKAGE_VERSION << std::endl; 
    //  return 0;
  }
  
  bool verbose = false; 

  std::string output_file = "";
  std::string  input_file = "";

  for ( int i=1 ; i<argc ; i++ ) { 
    if      ( std::string(argv[i])=="--verbose" ) verbose = true;
     else if ( std::string(argv[i])=="-o" ) { 
      ++i;
      if ( i<argc ) output_file = argv[i];
      else  return usage( std::cerr, argc, argv );
    }
    else { 
      if ( input_file=="" ) input_file = argv[i];
      else                  return usage( std::cerr, argc, argv ); 
    }
  }


  /// if output file is not defined
  if ( output_file == "" ) return usage( std::cerr, argc, argv );
  if ( input_file == "" )  return usage( std::cerr, argc, argv );
  
  //  dataset data("test_EF");
  //  files = data.datafiles();
  
  /// check som input files


  //  time the actual running of the cost() routine
 
  if ( verbose ) std::cout << "timing" << std::endl;

  struct timeval tv = simpletimer_start();

  cost( input_file, output_file );

  double t = simpletimer_stop(tv);

  std::cout << "\ntotal time " << t*0.001 << " s  (" << (t*0.001/60) << " min)" << std::endl;

  return 0;
}
