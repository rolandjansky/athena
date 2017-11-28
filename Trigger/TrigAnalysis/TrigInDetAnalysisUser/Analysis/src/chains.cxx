
#include <iostream>
#include <string>


#include "TFile.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TH1D.h"



void descend( TDirectory* d, int level, int maxdepth, const std::vector<std::string>& labels );




int usage(std::ostream& s, int argc, char** argv) { 
  if ( argc<1 ) return -1; /// should never be the case 
  s << "Usage: " << argv[0] << " [OPTIONS] <file1>  <file2>  ... \n\n";
  s << "  writes names of chains from files\n\n";
  s << "Options: \n";
  s << "    -a, --always             \t always descend, \n";
  s << "    -d, --depth    maxdepth  \t maximum depth to descend, \n";
  s << "    -c, --contents histogram \t display number of entries in histogram, \n";
  s << "    -f, --fast               \t bomb out as soon as a single chain has been found, \n";
  s << "    -v, --verbose            \t verbose output, \n";
  s << "    -h, --help               \t this help,\n";
  //  s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  //  s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
  s << std::endl;
  return 0;
}


/// very lazy!!! should not use unnessecary 
/// global variables !!!!

bool               verbose = false;
bool        always_descend = false;
std::string      histogram = "";
bool                  fast = false;

int main(int argc, char** argv ) { 

  std::vector<std::string> args;

  int maxdepth = 2;


  for ( int i=1 ; i<argc ; i++ ) { 
    std::string arg = argv[i];
    if      ( arg=="-h" || arg=="--help" )    return usage( std::cout, argc, argv );
    else if ( arg=="-v" || arg=="--verbose" ) verbose        = true;
    else if ( arg=="-f" || arg=="--fast" )    fast = true;
    else if ( arg=="-a" || arg=="--always" )  always_descend = true;
    else if ( arg=="-d" || arg=="--depth" )  { 
      if ( ++i<argc ) maxdepth = std::atoi(argv[i]);
      else            return usage( std::cout, argc, argv );
    }
    else if ( arg=="-c" || arg=="--contents" )  { 
      if ( ++i<argc ) histogram = argv[i];
      else            return usage( std::cout, argc, argv );
    }

    else args.push_back(arg);
  } 

  std::vector<std::string> labels;
  labels.push_back("L2_");
  labels.push_back("EF_");
  labels.push_back("HLT_");

  for ( unsigned i=0 ; i<args.size() ; i++ ) {
    TFile f( args[i].c_str() );

    if ( !f.IsOpen() || f.IsZombie() ) {
      std::cerr << "Error: could not open input file: " << args[i] << std::endl;
      std::exit(1);;
    }

    //    gDirectory->ls();
    descend( gDirectory, 0, maxdepth, labels );
  }

  return 0;
}

/// sum the bin contents including the over and underflow bins
double hsum( TH1* h ) { 
  double d=0;
  for ( int i=0 ; i<=h->GetNbinsX()+1 ; i++ ) d += h->GetBinContent(i);
  return d;
}

void descend( TDirectory* d, int level, int maxdepth, const std::vector<std::string>& labels ) { 

  if ( verbose ) std::cout << "level " << level << "\tmaxdepth " << maxdepth << std::endl;

  if ( d==0 ) return;

  TList* tl  = d->GetListOfKeys();

  if ( verbose ) std::cout << "list of keys " << tl << " " << ( tl ? tl->Capacity() : 0 ) << std::endl; 

  if ( tl==0 ) return;

  std::string name = d->GetName();

  int plevel = level;

  if ( !verbose ) if ( plevel>0 ) plevel--;  

  for ( int ip=plevel ; ip-- ; ) std::cout << "\t";
  
  bool displayed = false;

  int dsize = 0;

  if ( verbose )  std::cout << "Directory: ";
  if ( verbose || level>0 ) { 
    std::cout<< name;
    displayed = true;
    dsize = name.size();
    if ( dsize<60 ) std::cout << "\t";
  }

  //    struct timeval tv = simpletimer_start();
  
  if ( verbose ) std::cout << "level " << level << "\tmaxdepth " << maxdepth << std::endl;

  for ( int i=tl->GetSize() ; i-- ; ) { 
    
    TKey* tobj = (TKey*)tl->At(i);
    
    if ( tobj==0 ) continue;
  

    //    for ( int ip=level+1 ; ip-- ; ) std::cout << "\t";
    //    std::cout << "tobj " << tobj << " : \t" << tobj->GetName() << std::endl; 
    
    if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
      //      std::cout << ns << "Directory " << tobj->GetName() << std::endl;
      TDirectory* tnd = (TDirectory*)tobj->ReadObj();

      if ( tnd==0 ) continue;

      std::string dname = tnd->GetName();

      
      //      if ( dname.find("L2_")!=std::string::npos || 
      //	   dname.find("EF_")!=std::string::npos || 
      //	   dname.find("HLT_")!=std::string::npos )
      if ( level<maxdepth ) { 
	bool dscnd = true;
	if ( !always_descend && labels.size() ) { 
	  dscnd = false;
	  for ( unsigned il=labels.size() ; il-- ; ) { 
	    if ( dname.find(labels[il])!=std::string::npos) dscnd = true;
	  } 
	}
	if ( dscnd ) descend( tnd, level+1, maxdepth, labels );
      }
    }
    else { 
      if ( histogram != "" ){ 
        if ( std::string(tobj->GetClassName()).find("TH1")!=std::string::npos ||
             //  std::string(tobj->GetClassName()).find("TH2")!=std::string::npos ||
             std::string(tobj->GetClassName()).find("TProfile")!=std::string::npos ) {
          
          TH1* th = (TH1*)tobj->ReadObj();
          
          if ( th && std::string(th->GetName()) == histogram ) { 
 
            double d = hsum( th );
	    
	    std::cout << "\t" << d << std::endl;

	    displayed = false;

          }
        }
      }
    }
  }
  if ( displayed ) std::cout << std::endl;
}

