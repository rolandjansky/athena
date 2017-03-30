
#include <iostream>
#include <string>


#include "TFile.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"



void descend( TDirectory* d, int level, int maxdepth, const std::vector<std::string>& labels );




int usage(std::ostream& s, int argc, char** argv) { 
  if ( argc<1 ) return -1; /// should never be the case 
  s << "Usage: " << argv[0] << " [OPTIONS] <file1>  <file2>  ... \n\n";
  s << "  writes names of chains from files\n\n";
  s << "Options: \n";
  s << "    -a, --always   \t always descend, \n";
  s << "    -d, --depth  maxdepth  \t maximum depth to descend, \n";
  s << "    -v, --verbose  \t verbose output, \n";
  s << "    -h, --help     \t this help,\n";
  //  s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  //  s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
  s << std::endl;
  return 0;
}


/// very lazy!!! should not use unnessecary 
/// global variables !!!!

bool verbose        = false;
bool always_descend = false;

int main(int argc, char** argv ) { 

  std::vector<std::string> args;

  int maxdepth = 2;

  for ( int i=1 ; i<argc ; i++ ) { 
    std::string arg = argv[i];
    if      ( arg=="-h" || arg=="--help" )    return usage( std::cout, argc, argv );
    else if ( arg=="-v" || arg=="--verbose" ) verbose        = true;
    else if ( arg=="-a" || arg=="--always" )  always_descend = true;
    else if ( arg=="-d" || arg=="--depth" )  { 
      if ( ++i<argc ) maxdepth = std::atoi(argv[i]);
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
    //    gDirectory->ls();
    descend( gDirectory, 0, maxdepth, labels );
  }

  return 0;
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
  
  if ( verbose )  std::cout << "Directory: ";
  if ( verbose || level>0 ) std::cout<< name << std::endl; 

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
      //for ( int ip=level+1 ; ip-- ; ) std::cout << "\t";
      // std::cout << "Object: " << name << "/" <<  tobj->GetName() << std::endl;
    }
  }
}

