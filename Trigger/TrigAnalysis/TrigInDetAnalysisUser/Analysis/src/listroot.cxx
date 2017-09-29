
#include "simpletimer.h"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "TH1F.h"
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
#include "TPad.h"
#include "TStyle.h"

#include "TDirectory.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "utils.h"

#include "DrawLabel.h"
#include "label.h"

int ir = 0;

bool dirflag = true;
bool dbg     = true;

std::string cwd;

static TFile* fout = 0;

bool overlay = false;

bool plot = false;
bool logy = false;

bool drawmean = true;

double entries = 0;

int colours[4] = { kBlack, kRed, kBlue, kMagenta };
int styles[4]  = { 1, 2, 3, 4 };

int istyle = 0;

bool drawn = false;

bool binwidth = false;

double xmin = 0;
double xmax = 0;

double ymin = 0;
double ymax = 0;


bool contains( const std::string& s, const std::string& regex ) { return s.find(regex)!=std::string::npos; }





template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T*>& t ) { 
  for ( size_t i=0 ; i<t.size() ; i++ ) s << *t[i] << "\n";
  return s;
}  


void binWidth( TH1* h ) { 
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    double d = h->GetBinLowEdge(i+1)-h->GetBinLowEdge(i);
    h->SetBinContent( i, h->GetBinContent(i)/d );
    h->SetBinError( i, h->GetBinError(i)/d );
  }
}

template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& t ) { 
  for ( size_t i=0 ; i<t.size() ; i++ ) s << t[i] << "\n";
  return s;
}  

void minlog( TH1* h ) {
  double mn = 1;
  double mx = 1;
  bool unset = true;
  for ( int i=h->GetNbinsX()+1 ; i-- ;  ) { 
    double d = h->GetBinContent(i);
    if ( d!=0 && unset ) { 
      unset = false;
      mn = d;
      mx = d;
    }
    if ( d!=0 ) { 
      if ( d<mn ) mn = d;
      if ( d>mx ) mx = d;
    }
  }
  h->SetMinimum(mn*0.1);
  h->SetMaximum(mx*10);
}


double hmax( TH1*& h ) { 

  if ( h && h->GetEntries()>0 && h->GetNbinsX()>0 ) { 
  
  h->SetDirectory(0);

#if 0
  TH1D* nh = new TH1D( h->GetName(), h->GetTitle(), 
		       h->GetNbinsX(), h->GetBinLowEdge(1), h->GetBinLowEdge( h->GetNbinsX()+1 ) ); 
#endif

  TH1D* nh = (TH1D*)h->Clone();

  double n = 0;

    for ( int i=1 ; i<h->GetNbinsX()+1 ; i++ ) {
      n += h->GetBinContent(i);
      nh->SetBinContent( i, h->GetBinContent(i) );
    }
    
    nh->SetEntries( n ); 

    //    nh->GetXAxis()->SetTitle( h->GetXaxis()->GetTitle() );
    //    nh->GetYAxis()->SetTitle( h->GetYaxis()->GetTitle() );

    h = nh;

    

    double _hmax = h->GetBinContent(1);
    for ( int i=2 ; i<h->GetNbinsX() ; i++ ) {
      double _m = h->GetBinContent(i);
      if ( _m>_hmax ) _hmax = _m;
    }
    return _hmax;
  }
  return 0;
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

std::vector<std::string> veto; /// these patterns are vetoed

double minmean = 0;
double maxmean = 1e10;

bool isvetoed( const std::string& s ) { 
  for ( size_t i=0 ; i<veto.size() ; i++ ) if ( contains(s,veto[i]) ) return true;
  return false;
}


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


    //    std::cout << "GOLLY! " << _dir->GetName() << "\tdepth " << depth << " " <<  __depth << "\tsize " << directories.size() << " " << directories << std::endl;


    if ( directories.size()>size_t(0) ) {
      if ( __depth>=0 && size_t(__depth)<directories.size() ) {
	//	std::cout << "directory[" << __depth << "] = " << directories[__depth] << " " << std::string(_dir->GetName()) << std::endl;
	if ( directories[__depth]!=std::string(_dir->GetName() ) &&
	     !( // contains( _dir->GetName(), "SMK_" ) &&
		contains( _dir->GetName(), directories[__depth] ) ) )  accept = false;
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

	  if ( accept && isvetoed(tobj->GetName()) ) accept = false;

	  if ( accept ) { 
	    _nodes.push_back( inode( tobj->GetName(), tobj->GetClassName(), tobj ) );


	    //	    bool display = true;
	  
	    std::cout << "(" << depth << ")\t" << spacer << _nodes.back(); // << " " << tobj->GetName();
	    
	    if ( std::string( tobj->GetClassName()).find("TH1")!=std::string::npos ) {
	      TH1* h = (TH1*)tobj->ReadObj();
	      h->SetDirectory(0);
	      //  bool _cck = false;


	      //	      if ( isvetoed(tobj->GetName()) ) plot = false;

	      if ( entries!=0 && h->GetEntries()>entries ) { 
		std::cout << "\tentries " << h->GetEntries() << " \tmean " << h->GetMean() << "\tplot " << plot << "\t"; // << std::endl;
		//	_cck = true;
	      }


	      if ( plot ) { 
		//		std::cout << "plotting " << tobj->GetName() << "\tmean " << h->GetMean() << "\tentries " << h->GetEntries() << "\tis vetoed" << isvetoed( tobj->GetName() ) << std::endl;
		if ( h->GetMean()>=minmean && h->GetMean()<=maxmean ) { 
		  //		  std::cout << "plotting " << tobj->GetName() << "\tmean " << h->GetMean() << std::endl;
		  std::cout << "plotting min " << minmean << "\tmax " << maxmean; // << tobj->GetName() << "\tmean " << h->GetMean() << std::endl;
		  h->SetMaximum( hmax(h)*1.2 );

		  std::string t = tobj->GetName();

		  if ( t.find("____")!=std::string::npos ) t.erase( 0, t.find("___")+4);
		  if ( t.find("_FirstTime")!=std::string::npos ) t.erase( t.find("_FirstTime"), t.size());

		  h->SetTitle(t.c_str());

		  if ( logy ) minlog( h );

		  h->SetLineColor( colours[istyle%4] );
		  h->SetLineStyle( styles[(istyle/4)%4] );

		  if ( binwidth ) binWidth( h );

		  if ( !drawn || !overlay ) { 
		    if ( xmax!=xmin ) h->GetXaxis()->SetRangeUser( xmin, xmax );
		    if ( ymax!=ymin ) { 
		      h->SetMaximum( ymax );
		      if ( logy && ymin!=0 )  h->SetMinimum( ymin );
		    }
		    h->DrawCopy("hist");
		  }

		  h->DrawCopy("samehist");


		  if ( drawmean ) DrawLabel( 0.5, 0.8-istyle*0.05, label("mean = %5.2lf #pm %5.2lf ms", h->GetMean(), h->GetMeanError() ), colours[istyle%4] );

		  if ( logy ) gPad->SetLogy(true);
		  gPad->Print( (std::string(tobj->GetName())+".pdf").c_str() );


		  if ( overlay ) istyle++; 
		      
		  drawn = true;
		}
	      }

	      std::cout << std::endl;

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
// int         TCck::depth = 0;



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

  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);

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
    else if ( arg=="-v" ) { 
      if (++i < argc)  veto.push_back( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--min" ) { 
      if (++i < argc)  minmean = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--max" ) { 
      if (++i < argc)  maxmean = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--entries" ) { 
      if (++i < argc)  entries = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="-P" ) { 
      if (++i < argc)  Patterns.push_back( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--xmax" ) { 
      if (++i < argc)  xmax = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--xmax" ) { 
      if (++i < argc)  xmax = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--xmin" ) { 
      if (++i < argc)  xmin = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--ymin" ) { 
      if (++i < argc)  ymin = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--ymax" ) { 
      if (++i < argc)  ymax = std::atof( argv[i] );  
      else             return -1; 
    }
    else if ( arg=="--plot" )     plot = true;
    else if ( arg=="--logy" )     logy = true;
    else if ( arg=="--overlay" )  overlay = true;
    else if ( arg=="--binwidth" ) binwidth = true;
    else files.push_back( arg );
    
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

    //    std::cout << "root is extremely annoying, why is it taking so long to close this file?" << std::endl;

    //    f->Close();

    std::cout << "done" << std::endl;

  }

  if ( fout ) {
    fout->Write();
    fout->Close();
  }

  return 0;
} 



