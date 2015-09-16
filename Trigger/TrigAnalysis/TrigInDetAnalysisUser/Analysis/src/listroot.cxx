
#include "simpletimer.h"

#include <string>
#include <iostream>

#include "TH1F.h"
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "utils.h"


int ir = 0;

bool dirflag = true;
bool dbg     = true;

std::string cwd;

static TFile* fout = 0;



template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T*>& t ) { 
  for ( size_t i=0 ; i<t.size() ; i++ ) s << *t[i] << "\n";
  return s;
}  


template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& t ) { 
  for ( size_t i=0 ; i<t.size() ; i++ ) s << t[i] << "\n";
  return s;
}  



class inode { 
public:
  inode( const std::string& n,  
	 const std::string& t, 
	 TKey* k, TDirectory* d=0, 
	 const std::vector<inode>& in=std::vector<inode>() ) 
    : _name(n), _type(t), _key(k), _dir(d), _nodes(in) { }  

  //  ~inode() { std::cout << "delete " << _name << std::endl; } 
  ~inode() { }

  std::string _name;
  std::string _type;
  TKey*       _key;

  /// if this node is, itself a directory
  TDirectory* _dir;
  std::vector<inode> _nodes;
}; 



std::ostream& operator<<( std::ostream& s, const inode& _t ) { 
  s << _t._name << " " << _t._type;
  //  if ( _t._dir ) s << "\n" << _t._nodes;
  return s;
}  



std::vector<std::string> directories; 

std::vector<std::string> patterns; /// these patterns are *ored*

std::vector<std::string> Patterns; /// these patterns are *anded* 


int maxdepth = 3;

class TCck { 
  
public:

  TCck( TDirectory* d, int depth=0 ) : _dir(d), _depth(depth) { } 

  //  ~TCck() { if ( _dir ) std::cout << "delete dir " << _dir->GetName() << std::endl; } 

  ~TCck() { } 
  
  std::vector<inode> scan( int depth=0 ) { 
    if ( _nodes.size()==0 ) _scan(depth);
    return _nodes;
  }

protected:
  
  void _scan( int depth=0 ) {

    if ( depth>maxdepth ) return;

    std::string inspace = spacer;
    
    TDirectory* cck = gDirectory;

    int __depth = depth-1;

    bool accept = true;


    //    std::cout << "FUCK " << _dir->GetName() << "\tdepth " << depth << " " <<  __depth << "\tsize " << directories.size() << " " << directories << std::endl;


    if ( directories.size()>0 ) {
      if ( __depth>=0 && unsigned(__depth)<directories.size() ) {
	//	std::cout << "directory[" << __depth << "] = " << directories[__depth] << " " << std::string(_dir->GetName()) << std::endl;
	if ( directories[__depth]!=std::string(_dir->GetName() ) )  accept = false;
      }
    }

    //    std::cout << "accept " << accept << std::endl;


    if ( !accept ) return;

    //    std::cout << spacer << _dir->GetName() << "\tdepth " << _depth << std::endl; 
  
    spacer += "\t";


    _dir->cd();


    TList* tl  = _dir->GetListOfKeys();


    for ( int i=tl->GetSize() ; i-- ; ) { 

      TKey* tobj = (TKey*)tl->At(i);
      if ( tobj ) {
	TDirectory* tnd = 0;
	std::vector<inode> nodes;
	
	//	if ( depth>1 ) continue;
	
	//	std::cout << "(" << depth << ")\t" << spacer << tobj->GetName() << "\t: " << tobj->GetClassName() << std::endl;

	if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
	  tnd = (TDirectory*)tobj->ReadObj();
	  nodes = TCck( tnd, depth+1 ).scan( depth+1 );
	  _nodes.push_back( inode( tobj->GetName(), tobj->GetClassName(), tobj, tnd, nodes ) );
	}
	else { 
	  
	  bool accept = false;

	  if ( patterns.empty() ) accept = true;
	  else { 
	    for ( unsigned ip=patterns.size() ; ip-- ; ) { 
	      if ( std::string( tobj->GetName()).find( patterns[ip] )!=std::string::npos ) accept = true;
	      // if ( accept ) std::cout << "match " << patterns[ip] << std::endl;
	    }
	  }

	  for ( unsigned ip=Patterns.size() ; ip-- ; ) { 
	    if ( std::string( tobj->GetName()).find( Patterns[ip] )==std::string::npos ) accept = false;
	    // if ( accept ) std::cout << "match " << Patterns[ip] << std::endl;
	  }


	  if ( accept ) { 
	    _nodes.push_back( inode( tobj->GetName(), tobj->GetClassName(), tobj ) );
	    std::cout << "(" << depth << ")\t" << spacer << _nodes.back(); // << " " << tobj->GetName();
	    
	    if ( std::string( tobj->GetClassName()).find("TH1")!=std::string::npos ) {
	      TH1* h = (TH1*)tobj->ReadObj();
	      h->SetDirectory(0);
	      // bool _cck = false;
	      //	      if ( h->GetEntries()>5000 ) { 
	      std::cout << "\tentries " << h->GetEntries() << " \tmean " << h->GetMean() << std::endl;
	      // _cck = true;
	      //  }
	      //  else std::cout << std::endl;

	      if ( fout ) { 
		std::string cckname = tobj->GetName();
		TDirectory* cck = gDirectory;
		fout->cd();
		
		std::string tdir = chop( cckname, "____" );

		if ( cckname.size()==0 ) { 
		  h->SetName( tdir.c_str() );
		  h->Write();
		}
		else { 
		  TIDDirectory dcck(tdir);
		  dcck.push();
		  		 
		  h->SetName( cckname.c_str() );
		  h->Write();
		  dcck.pop();
		}
		cck->cd();
	      }
	    }
	    else { 
	      std::cout << std::endl;
	    }
	  }
	}
      }
    }      
    
    if ( cck ) cck->cd();

    spacer = inspace;
  }

protected:
  
  TDirectory*        _dir;
  std::vector<inode> _nodes;

  int                _depth;

  static std::string spacer;
  
};



std::string TCck::spacer = "";



std::vector<std::string> split( std::string s, const std::string& regex ) { 

  std::vector<std::string> out;
  
  size_t pos = s.find(regex);

  while ( pos!=std::string::npos ) { 
    out.push_back( s.substr( 0, pos ) );
    s.erase( 0, pos+1 ); 
    pos = s.find(regex);
  }

  out.push_back( s );

  return out;

}







void scan(TDirectory* td=0, int depth=0 ) { 
  
  if ( td ) { 
    TCck t( td, depth ); 
    std::cout << t.scan( depth ) << std::endl;
  }

  //     if ( std::string(tobj->GetClassName()).find("TH1")!=std::string::npos )       add<TH1>( objname.c_str(), tobj, depth+"\t" );
  //     if ( std::string(tobj->GetClassName()).find("TH2")!=std::string::npos )       add<TH2>( objname.c_str(), tobj, depth+"\t" );
  //     if ( std::string(tobj->GetClassName()).find("TProfile")!=std::string::npos )  add<TProfile>( objname.c_str(), tobj, depth+"\t" );

}



int main(int argc, char** argv) { 

  std::vector<std::string> files;

  std::string dir = "";

  /// handle arguments 

  std::string outfile = "";

  for ( int i=1 ; i<argc ; i++ ) {

    std::string arg = argv[i];

    if ( arg=="-o" ) { 
      if (++i < argc)  outfile = argv[i];  
      else             return -1; 
    }
    else if ( arg=="--depth" ) { 
      if (++i < argc)  maxdepth = std::atoi(argv[i]);  
      else             return -1; 
    }
    else if ( arg=="-d" ) { 
      if (++i < argc)  dir = argv[i];  
      else             return -1; 
    }
    else if ( arg=="-p" ) { 
      if (++i < argc)  patterns.push_back( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="-P" ) { 
      if (++i < argc)  Patterns.push_back( argv[i] );  
      else             return -1; 
    }
    else { 
      files.push_back( arg );
    }
  }


  if ( dir!="" ) directories = split( dir, "/" );

  /// navigate files

  if ( outfile!="" ) { 
    fout = new TFile( outfile.c_str(), "recreate" );
    std::cout << "saving to file: " << outfile << std::endl;
  }


  for ( size_t i=0 ; i<files.size() ; i++ ) { 

    TFile* f = new TFile( files[i].c_str() );
    f->cd();

    scan( gDirectory, 0 ); //, dir );

    std::cout << "closing" << std::endl;

    std::cout << "why does root take so long to close this file ?!!?" << std::endl;

    //    f->Close();

    std::cout << "done" << std::endl;

  }

  if ( fout ) {
    fout->Write();
    fout->Close();
  }

  return 0;
} 



