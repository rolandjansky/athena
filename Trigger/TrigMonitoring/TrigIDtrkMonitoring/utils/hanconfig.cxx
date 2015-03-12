//
//   @file    hanconfig.cxx         
//            navigates through the directory structure of a monitoring 
//            root files and write out some han config boiler plate
//            
//   @author M.Sutton
// 
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: hanconfig.cxx, v0.0   Thu  12 March 2015 14:13:47 CEST sutt $




#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include <stdio.h>

#include "TKey.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TClass.h"

#include "simpletimer.h"


// #include "Resplot.h"
// #include "TrigInDetAnalysis/Efficiency.h"

/// file names and file pointers
std::vector<std::string> files;
std::vector<TFile*>      fptr;

bool dbg = false;


bool dirflag = false;

/// glabal timer - how long have I taken so far?
struct timeval global_timer;


/// tag string for intendation
std::string depth = ""; 

/// counter of the current depth
int ir=0;


std::string tail( const std::string& s ) { 
  
  std::string _s = s;
  size_t pos = _s.find("/");
  while ( pos!=std::string::npos ) { 
    _s.erase( 0, pos+1 );
    pos = _s.find("/");
  }
  return _s;
}


std::string chop( const std::string& s ) {   
  std::string _s = s;
  size_t pos = _s.find("/");
  if ( pos!=std::string::npos ) _s.erase( 0, pos+1 );
  return _s;
}


std::string cwd = "";

template<class T>
bool add( const std::string& hname, TKey* tobj, const std::string depth="" ) { 
  
  T* h0 = (T*)tobj->ReadObj();

  //  std::cout << "h0 " << h0;
  //  if ( h0 ) std::cout << " : " << h0->GetName();
  //  std::cout << std::endl;
  
  bool changed = false;
  
  if ( h0==0 ) { 
    std::cerr << "object " << hname << " can't be found" << std::endl;
    return false;
  }

  if ( h0->GetSumw2N()==0 ) h0->Sumw2();

  if ( dbg ) std::cout << depth << "found object " << hname << std::endl;

  if ( dirflag ) { 
    std::cout << depth << "hist " << tail(hname) << " {\n";
    {
      std::cout << depth << "\talgorithm = HLTidtrkHistogram_Not_Empty&GatherData\n";
      std::cout << depth << "\tdescription = https://twiki.cern.ch/twiki/bin/view/Atlas/HltTrackingDataQualityMonitoring#Tier0\n";
      std::cout << depth << "\toutput = " << chop(cwd) << "\n";
      std::cout << depth << "\tweight = 0.0\n";
      if ( hname.find("Eff_")==std::string::npos ) std::cout << depth << "\tdisplay = StatBox\n";    
    }
    std::cout << depth << "}\n";
  }

  delete h0;

  return true;

  static int count = 0;

  for ( unsigned ifile=1 ; ifile<fptr.size() ; ifile++ ) { 
    
    //    std::cout << "TFile* fptr " << fptr[ifile] << " " << files[ifile] << std::endl;

    if ( fptr[ifile] ) {  

      //      std::cout << "h (get) >" << hname << "<" << std::endl; 

      T* h  = (T*)fptr[ifile]->Get(hname.c_str());

      

      if ( h==0 ) {
	std::cerr << "h (add) " << h << "  " << hname << " can't be opened" << std::endl; 
	if ( count>20 ) std::exit(-1); 
	count++;
	continue;
      }

      if ( h->GetSumw2N()==0 ) h->Sumw2();

      //      std::cout << "h       " << h->GetName() << std::endl; 

      changed = true;
      // h0->Add( h );

      delete h;
      
    }
  }
  
  
  
  //  if ( changed ) { 
  //   h0->SetDirectory(0);
  //   h0->Write("",TObject::kOverwrite);
  //   //    h0->Write(); // "",TObject::kOverwrite);
  //  }
  
  delete h0;
 
  return true;
}






/// recursive directory search for TH1 and TH2

void search(TDirectory* td=0, const std::string& s="") { 

  ir++;

  /// don;t go more than 10 directories deep

  if ( ir>10 ) std::exit(0);


  std::string savedepth = depth;
  
  depth += "\t";

  TDirectory* here = gDirectory;

  //  gDirectory->pwd();

  std::string ns = s;

  if ( ns!="" ) ns += "/"; 

  if ( td )  { 
    gDirectory->cd(td->GetName());

    ns +=  td->GetName();

    if ( dbg ) std::cout << depth <<  "\033[91;1m" << "- Directory " << ns << "\033[m\n";
    
    //    if ( !dirflag )  std::cout << depth <<  "output " << ns << "{\n";
    if ( !dirflag )  std::cout << depth <<  "output " << tail(ns) << "{\n";
    else             std::cout << depth <<  "dir " << tail(ns) << "{\n"; 
  }

  cwd = ns;

  //  std::cout << ir << " " << ns << "TDirectory " << gDirectory->GetPath() << " " << gDirectory->GetName() << std::endl;

  TList* tl  = gDirectory->GetListOfKeys();

  struct timeval tv = simpletimer_start();

  //  std::cout << "\ttl " << tl << std::endl;

  for ( int i=tl->GetSize() ; i-- ; ) { 

    TKey* tobj = (TKey*)tl->At(i);

    if ( tobj==0 ) continue;

    //    std::cout << "tobj " << tobj;
    //    if ( tobj ) std::cout << " : \t" << tobj->GetName();
    //    std::cout << std::endl; 
    
    if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
      //      std::cout << ns << "Directory " << tobj->GetName() << std::endl;
 
      TDirectory* tnd = (TDirectory*)tobj->ReadObj();

      if ( tnd ) search( tnd, ns );
    }
    else { 
      //      bool status = false;
      
      std::string objname = ns;
      if ( objname!="" ) objname += std::string("/") + tobj->GetName();
      
      //      if ( std::string(tobj->GetClassName()).find("TH1")!=std::string::npos )  status = add<TH1>( objname.c_str(), tobj );
      //      if ( std::string(tobj->GetClassName()).find("TH2")!=std::string::npos )  status = add<TH2>( objname.c_str(), tobj );
      if ( std::string(tobj->GetClassName()).find("TH1")!=std::string::npos )  add<TH1>( objname.c_str(), tobj, depth+"\t" );
      if ( std::string(tobj->GetClassName()).find("TH2")!=std::string::npos )  add<TH2>( objname.c_str(), tobj, depth+"\t" );
      
      //      if ( !status ) std::cerr << "bad status" << std::endl;
    }

  }

  double _t = simpletimer_stop(tv);
  
  double global_time = simpletimer_stop(global_timer);

  
  if ( dbg ) { 
    std::cout << depth <<  "\033[91;1m" << "  Directory " << ns << "\033[m";
    std::cout << "\033[91;1m\tprocessed directory in " << _t*0.001 << " s  from " << global_time*0.001 << " s\033[m\n";
  }

  if ( !dirflag )  std::cout << depth <<  "}\n";
  else             std::cout << depth <<  "}\n";

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




void cost( std::vector<std::string>& files, const std::string& directory="" ) { 

  std::cout << "processing" << std::endl;

  /// don't over write esisting files
  //  if ( file_exists(outputfile) ) { 
  //   std::cerr << "output file " << outputfile << " already exists" << std::endl;
  //   return;
  //  }

  //  std::cout << "copying file to " << outputfile << std::endl;

  /// copy the first file so we add to this copy
  //  system( ("cp "+files[0]+" "+outputfile).c_str() );


  /// don't bother to open the first file - we've copied 
  /// it to the output and will update the contents

  std::cout << "opening root files" << std::endl;

  fptr.resize(files.size());


  std::cout << "opening " << files[0] << std::endl;

  for ( unsigned i=0 ; i<files.size() ; i++ ) { 

  /// open the output file
    fptr[i] = new TFile( files[i].c_str() );
    
    if ( fptr[i]==0 ) { 
      std::cerr << "cannot open " << files[i] << std::endl;
      return;
    }

    if ( fptr[i]==0 ) { 
      std::cerr << "file " << files[i] << " cannot be openened" << std::endl;
      return;
    }
  
    fptr[i]->cd();
  
    if ( directory!="" ) fptr[i]->cd(directory.c_str());

    gDirectory->pwd();

    TDirectory* here = gDirectory;

    global_timer = simpletimer_start();

    /// recursively look through the directory
    /// structure   
    search();
    
    here->cd();
    
    std::cout << "closing the files" << std::endl; 
    
    
    /// don't need to write the histogram again - writing them as we go along
    //  f0->Write();
    fptr[i]->Close();

    delete fptr[i];
  }

}




int usage(std::ostream& s, int , char** argv) { 
  s << "Usage: " << argv[0] << " [OPTIONS] input1.root ... inputN.root\n\n";
  s << "     filename   \tname of output (filename required)\n";
  s << "    -d,   --dir DIR  \tonly directories below DIR\n";
  s << "    -dbg, --dbg      \tprint debugging information\n";
  s << "    -h,   --help     \tdisplay this help\n";
  s << std::endl;
  return 0;
}




int main(int argc, char** argv) { 

  //  if ( argc<3 ) usage( std::cerr << "not enough command options", argc, argv );
  if ( argc<2 ) return usage( std::cerr, argc, argv );


  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" )  return usage( std::cout, argc, argv ); 
    //    if ( std::string(argv[i])=="-v" || std::string(argv[i])=="--version" ) {
    //      std::cout << argv[0] << " APPLgrid version " << PACKAGE_VERSION << std::endl; 
    //  return 0;
  }
  
  bool verbose = false; 

  std::string directory = "";

  for ( int i=1 ; i<argc ; i++ ) { 
    if      ( std::string(argv[i])=="--verbose" ) verbose = true;
    else if ( std::string(argv[i])=="-dbg" || std::string(argv[i])=="--dbg" ) dbg = true;
    else if ( std::string(argv[i])=="-d"   || std::string(argv[i])=="--dir" ) {
      ++i;
      if ( i<argc ) directory = argv[i];
      else  return usage( std::cerr, argc, argv );
    } 
    //    else if ( std::string(argv[i])=="-o" ) { 
    //    ++i;
    //    if ( i<argc ) output_file = argv[i];
    //   else  return usage( std::cerr, argc, argv );
    //   }
    else { 
      files.push_back(argv[i]);
    }
  }


  /// if output file is not defined
  //  if ( output_file == "" ) return usage( std::cerr, argc, argv );
  
  //  dataset data("test_EF");
  //  files = data.datafiles();
  
  /// check som input files


  if ( files.size()<1 ) return usage( std::cerr, argc, argv );

  
  //  time the actual running of the cost() routine
  
  if ( verbose ) std::cout << "timing" << std::endl;

  struct timeval tv = simpletimer_start();

  dirflag = false;
  cost(files, directory);


  dirflag = true;
  cost(files, directory);


  double t = simpletimer_stop(tv);

  std::cout << "\ntotal time " << t*0.001 << " s  (" << (t*0.001/60) << " min)" << std::endl;

  return 0;
}
