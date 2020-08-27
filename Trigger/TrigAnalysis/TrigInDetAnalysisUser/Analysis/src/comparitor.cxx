/**
 **     @file    comparitor.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 12 Oct 2012 13:39:05 BST 
 **
 **     Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h> 

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include "TrigInDetAnalysis/Efficiency.h"

#include "ReadCards.h"

#include "Resplot.h"
#include "utils.h"
#include "label.h"
#include "DrawLabel.h" 

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TF1.h"
#include "TPave.h"
#include "TPaveStats.h"
#include "TColor.h"

#include "computils.h"

#include "AtlasStyle.h"
#include "AtlasLabels.h"

#include "default_panels.h"

bool fulldbg = false;


extern bool LINEF;
extern bool LINES;


TH1F* Rebin( TH1F* h, double f ) { 
  
  if ( int(f) == f ) { 
    TH1F* n = (TH1F*)h->Clone("foeda"); n->SetDirectory(0);
    n->Rebin(int(f));
    return n;
  }

  int     fi=int(f);
  double fb=(f-fi);

  for ( int i=0 ; i<10 ; i++ ) { 
    fb *= 10; 
    if ( std::fabs(int(fb)-fb)<1e-6 ) break; 
  }

  std::vector<double> limits;
  std::vector<double> contents;
    
  for ( int i=1 ; i<=h->GetNbinsX()+1 ; i++ ) { 
    limits.push_back( h->GetBinLowEdge(i) );
    contents.push_back( h->GetBinContent(i) );
    if ( i<fb ) { 
      for ( int ib=1 ; ib<fi ; ib++ ) contents.back() += h->GetBinContent(++i);
    }
  }
  
  TH1F* n = new TH1F( "foeda", h->GetTitle(), limits.size()-1, &limits[0] ); n->SetDirectory(0);
  
  for ( size_t i=0 ; i<contents.size() ; i++ ) n->SetBinContent( i+1, contents[i] );

  return n;
}




TH1F* rebin_log( TH1F* h, bool low=false ) { 

  double limits[40] = {
    0.5, 1.5, 2.5, 3.5, 4.5, 
    5.5, 6.5, 7.5, 8.5, 9.5, 
    10.5, 11.5, 12.5, 13.5, 14.5, 
    15.5, 16.5, 17.5, 18.5, 19.5, 
    20.5, 21.5, 22.5, 23.5, 24.5, 
    25.5, 26.5, 28.5, 30.5, 32.5, 
    35.5, 38.5, 42.5, 46.5, 50.5, 
    55.5, 60.5, 66.5, 72.5, 78.5 
  };

  double lowlimits[24] =  { 
    0.5, 1.5, 2.5, 3.5, 4.5, 
    5.5, 6.5, 7.5, 8.5, 9.5, 
    10.5, 11.5, 12.5, 14.5, 17.5, 
    20.5, 24.5, 29.5, 35.5, 42.5, 
    50.5, 60.5, 72.5, 86.5, 
  };

  TH1F* n;

  if ( low ) n = new TH1F( "foeda", h->GetTitle(), 39, limits ); 
  else       n = new TH1F( "foeda", h->GetTitle(), 23, lowlimits );

  n->SetDirectory(0);

  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    n->Fill( h->GetBinCenter(i), h->GetBinContent(i) ); 
  }


  for ( int i=1 ; i<=n->GetNbinsX() ; i++ ) { 
    n->SetBinError(i, std::sqrt( n->GetBinContent(i) ) ); 
  }

  return n;
}


/// global typedef here is ok, since this is a standalone 
/// executable, including a main(). As such it does not need 
/// to be in a namespace at this point as it won't in any 
/// case be visible outside this translation unit

typedef std::map<std::string,std::string> chainmap_t;


/// get a histogram given a path, and an optional initial directory
/// if histogram is not found, then check for dir/name,
/// allows optional remapping of the reference chain information

template<typename T=TH1F>
T* Get( TFile& f, const std::string& n, const std::string& dir="",
	const chainmap_t* chainmap=0, std::vector<std::string>* saved=0 ) { 

  std::string name;

  size_t pos = n.find("+");
  if ( pos!=std::string::npos ) name = n.substr( 0, pos );
  else                          name = n;

  T* h = (T*)f.Get( name.c_str() );
  if ( h || dir=="" || name.find(dir)!=std::string::npos ) { 
    std::cout << "Get() name 0 " << name << " :: " << h << std::endl;
  }
  else { 
    name = dir+"/"+name;
    h = (T*)f.Get( name.c_str() );
    std::cout << "Get() name 1 " << name << " :: " << h << std::endl;
  }

  if ( h == 0 ) { 
    if ( chainmap && chainmap->size()!=0 ) { 
      for ( chainmap_t::const_iterator itr=chainmap->begin() ; itr!=chainmap->end() ; itr++ ) { 
      	if ( contains( name, itr->first ) ) { 
	  std::cout << "\tmatch: " << itr->first << " -> " << itr->second << std::endl;
	  name.replace( name.find(itr->first), itr->first.size(), itr->second );
	  h = (T*)f.Get( name.c_str() );
	  break;
	}
      }
    }
  }

  if ( saved ) saved->push_back( name );

  if ( h ) h->SetDirectory(0);

  return h;
}



class bands { 

public:

  bands() { } 

  bands( const bands& b ) : m_limits(b.m_limits), m_labels(b.m_labels) { }  

  bands( std::vector<double> limits, std::vector<std::string> labels ) 
  : m_limits(limits), m_labels(labels)
  {   }

  void range( const std::string& chain, TH1F* h ) {
    for ( size_t i=0 ; i<m_labels.size() ; i++ ) { 
      if ( chain.find(m_labels[i])!=std::string::npos ) return range( i, h );
    }
    return range( m_labels.size(), h );
  } 


  void range( size_t i, TH1F* h ) { 
  
    double minx = h->GetBinLowEdge(1);
    double maxx = h->GetBinLowEdge(h->GetNbinsX()+1);
 
    if      ( i>=m_limits.size() )  { maxx+=1; minx=maxx; }
    else if ( (i+1)==m_limits.size() ) minx=m_limits[i];
    else { 
      minx = m_limits[i];
      maxx = m_limits[i+1];
    }
  

    for ( int j=1 ; j<=h->GetNbinsX() ; j++ ) { 

      std::cout << "range: " << j << "\tminx: " << minx << "\t" << maxx << std::endl; 
      if ( ! ( h->GetBinCenter(j)>=minx && h->GetBinCenter(j)<maxx ) ) {
	h->SetBinContent(j,0);
	h->SetBinError(j,0);
      }
    }
  }
  
  
private: 

  std::vector<double>      m_limits;  
  std::vector<std::string> m_labels;  

};


int usage(const std::string& name, int status, const std::string& err_msg="" ) { 
  if ( err_msg != "" ) std::cerr << err_msg << "\n" << std::endl;
  std::ostream& s = std::cout;
  s << "Usage: " << name << "\t [OPTIONS]  test.root reference.root    chain1 chain2 chain2 ...\n\n"; 
  s << "\t" << " plots comparison histograms"; 
  s << " - compiled on " << __DATE__ << " at " << __TIME__ << "\n\n";
  s << "Options: \n";
  s << "    -c,  --config       value \t configure which histograms to plot from config file,\n\n";
  s << "    -t,  --tag          value \t appends tag 'value' to the end of output plot names, \n";
  s << "    -k,  --key          value \t prepends key 'value' to the front of output plots name, \n";
  s << "    -t,  --tag          value \t post pend tag 'value' to the end of output plots name, \n";
  s << "    -d,  --dir          value \t creates output files into directory, \"value\" \n\n";
  s << "         --ncols        value \t creates panels with \"value\" columns\n\n";

  s << "    -e,  --efficiencies       \t make test efficiencies with respect to reference \n";
  s << "    -es, --effscale     value \t scale efficiencies to value\n";
  s << "    -er, --effscaleref  value \t scale reference efficiencies to value\n";
  s << "    -nb  --nobayes            \t do not calculate Basyesian efficiency uncertaintiesr\n\n";

  s << "    -r,  --refit              \t refit all test resplots\n";
  s << "    -rr, --refitref           \t also refit all reference resplots\n";
  s << "         --oldrms             \t use fast rms95 when refitting resplots\n\n";

  s << "    -as, --atlasstyle         \t use ATLAS style\n";
  s << "    -l,  --labels       values\t use specified labels for key\n";
  s << "         --taglabels    values\t use specified additional labels \n";
  s << "    -al, --atlaslable   value \t set value for atlas label\n";
  s << "    -ac, --addchains          \t if possible, add chain names histogram labels \n\n";   

  s << "    -m,  --mapfile            \t remap file for reference histograms \n\n";   
 
  s << "    -rc, --refchains values ..\t allow different reference chains for comparison\n";
  s << "    -s,  --swap pattern regex \t swap \"pattern\" in the reference chains name by \"regex\"\n";
  s << "    -nr, --noref              \t do not plot reference histograms\n";
  s << "         --normref            \t normalise the reference counting histograms to test histograms\n";
  s << "    -us, --usechainref        \t use the histograms from chain definied in the \"Chain\" histogram as reference\n\n";

  s << "    -ns, --nostats            \t do not show stats for mean and rms\n";
  s << "    -nm, --nomeans            \t do not show stats for the means\n";
  s << "         --chi2               \t show the chi2 with respect to the reference\n\n";

  s << "    -np, --noplots            \t do not actually make any plot\n";
  s << "    -q,  --quiet              \t make the plots but do not print them out\n\n";

  s << "         --unscalepix         \t do not scale the number of pixels by 0.5 (unscaled by default)\n";
  s << "         --scalepix           \t do scale the number of pixels by 0.5 (unscaled by default)\n";
  s << "         --yrange     min max \t use specified y axis range\n";  
  s << "    -xo, --xoffset      value \t relative x offset for the key\n"; 
  s << "    -yp, --ypos         value \t relative yposition for the key\n"; 
  s << "    -xe, --xerror       value \t size of the x error tick marks\n"; 
  s << "    -nw, --nowatermark        \t do not plot the release watermark\n\n"; 

  s << "    -C,  --Cfiles             \t write C files also\n"; 
  s << "         --nopng              \t do not print png files\n"; 
  s << "         --nopdf              \t do not print pdf files\n"; 
  s << "         --deleteref          \t delete unused reference histograms\n\n";

  s << "         --printconfig        \t print the configuration being used in the form useable as an input file\n\n";

  s << "    -h,  --help              \t this help\n";
  //  s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  //  s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
  s << std::endl;
  return status;
}


void binwidth( TH1F* h ) { 
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    double w = h->GetBinLowEdge(i+1) - h->GetBinLowEdge(i);
    h->SetBinContent( i, h->GetBinContent(i)/w );
    h->SetBinError( i, h->GetBinError(i)/w );
  }
}

void ascale( TH1F* h, double s_ ) { 
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    h->SetBinContent( i, h->GetBinContent(i)*s_ );
    h->SetBinError( i, h->GetBinError(i)*s_ );
  }
}
 


// replace from a string
std::string fullreplace( std::string s, const std::string& s2, const std::string& s3) {
  if ( s2=="" || s2==s3 ) return s;  /// cowardly, don't replace string by itself 
  std::string::size_type pos;
  while ( (pos=s.find(s2)) != std::string::npos )  s.replace(pos, s2.size(), s3);
  return s;
} 

void print_pad( const std::string s ) { 
  std::cout << "Printing " << s << std::endl;
  gPad->Print( s.c_str() );
}



/// zero the contents of a 2d histogram 
void zero( TH2* h ) { 
  int Nx = h->GetNbinsX();
  int Ny = h->GetNbinsY();
  for ( int i=1 ; i<=Nx ; i++ ) { 
    for ( int j=1 ; j<=Ny ; j++ ) { 
      if ( h->GetBinContent( i, j )==0 ) { 
	h->SetBinContent( i, j, 1e-10 );
	h->SetBinError( i, j, 1e-10 );
      }
    }
  }
}

double chi2( TH1* h0, TH1* h1 ) { 
  double c2 = 0;

  for ( int i=0 ; i<h0->GetNbinsX() ; i++ ) {
 
    double d0 = h0->GetBinContent(i+1);
    double d1 = h1->GetBinContent(i+1);
 
    double e0 = h0->GetBinError(i+1);
    double e1 = h1->GetBinError(i+1);

    double  e2 = e0*e0+e1*e1;
    
    if ( e2>0 ) c2 += (d0-d1)*(d0-d1)/e2; 

  }
  
  return c2;
}


int main(int argc, char** argv) { 

  std::cout << "\n---------------------------------\n";
  std::cout << "\n comparitor is off ...\n";

  if ( argc<4 ) return usage(argv[0], -1, "too few arguments");

  /// control stuff
  
  std::string tag = "";
  std::string key = "";

  std::string dir = "";

  std::string ftestname = "";
  std::string frefname  = "";

  TFile* ftest_ = 0;
  TFile* fref_  = 0;

  bool   effset = false;
  double effmin =  90;
  double effmax = 102;


  /// control flags and labels

  std::vector<std::string> usrlabels;
  bool uselabels    = false;
  bool addinglabels = false;

  std::vector<std::string> taglabels;
  bool addingtags  = false;

  bool make_ref_efficiencies = false;
  bool refit_resplots        = false;
  bool refitref_resplots     = false;
  bool _bayes      = true;
  bool nostats     = false;
  bool nomeans     = false;
  bool noref       = false;
  bool atlasstyle  = false;
  bool deleteref   = false;
  bool nowatermark = false;
  bool noplots     = false;
  bool nopng       = false;
  bool nopdf       = false;
  bool Cfile       = false;
  bool notitle     = true;
  bool dochi2      = false;
  bool normref     = false;
  bool scalepix    = false;
  bool oldrms      = false;
  bool addchains   = false;
  bool usechainref = false;
  bool quiet       = false;

  bool RANGEMAP    = false;
  bool ALLRANGEMAP = false;

  double xerror    = 0;

  std::string atlaslabel_tmp = "Internal";


  double scale_eff     = -1;
  double scale_eff_ref = -1;
  
  std::string configfile = "";

  double xoffset = 0;

  
  double _ypos = 0;

  std::string pattern = "";
  std::string regex   = "";

  std::vector<std::string> chains;
  std::vector<std::string> refchains;

  bool addingrefchains = false;

  std::string mapfile = "";

  std::vector<std::string> chainfiles;

  int ncols = 2;

  for(int i=1; i<argc; i++){
    std::string arg  = argv[i];


    if ( arg.find("-")!=0 && addinglabels ) {
      std::string label = arg;
      replace( label, "#", " " );
      usrlabels.push_back( label );
      continue;
    }
    else addinglabels = false;

    if ( arg.find("-")!=0 && addingrefchains ) { 
      refchains.push_back( arg );
      continue;
    }
    else addingrefchains = false;

    if ( arg.find("-")!=0 && addingtags ) { 
      taglabels.push_back( fullreplace( arg, "__", " " ) );
      std::cout << "\tadding tag label: " << taglabels.back() << std::endl;
      continue;
    }
    else addingtags = false;

    if ( arg=="-h" || arg=="--help" ) { 
      return usage(argv[0], 0);
    }
    else if ( arg=="-c" || arg=="--config" ) { 
      if ( ++i<argc ) configfile=argv[i];
      else return usage(argv[0], -1, "no config file provided");
    }
    else if ( arg=="-t" || arg=="--tag" ) { 
      if ( ++i<argc ) tag=std::string("-")+argv[i];
      else return usage(argv[0], -1, "no tag provided");
    }
    else if ( arg=="-l" || arg=="--labels" ) { 
      addinglabels = true;
    }
    else if ( arg=="-el" ) { 
      LINES = true;
    }
    else if ( arg=="-k" || arg=="--key" ) { 
      if ( ++i<argc ) key=argv[i];
      else return usage(argv[0], -1, "no key provided");
    }
    else if ( arg=="-m" || arg=="--mapfile" ) { 
      if ( ++i<argc ) mapfile=argv[i];
      else return usage(argv[0], -1, "no mapfile provided");
    }
    else if ( arg=="-d" || arg=="--dir" ) { 
      if ( ++i<argc ) dir=argv[i];
      else return usage(argv[0], -1, "no directory provided");
    }
    else if ( arg=="--taglabels" ) { 
      addingtags = true;
    }
    else if ( arg=="--unscalepix" ) { 
      scalepix = false;
    }
    else if ( arg=="--scalepix" ) { 
      scalepix = true;
    }
    else if ( arg=="-ac" || arg=="--addchains" ) { 
      addchains = true;
    }
    else if ( arg=="-yrange" ) { 
      effset = true;
      if ( ++i<argc ) effmin=std::atof(argv[i]);
      else return usage(argv[0], -1, "no range specified");
      if ( ++i<argc ) effmax=std::atof(argv[i]);
      else return usage(argv[0], -1, "no upper y limit specified");
    }
    else if ( arg=="-e" || arg=="--efficiencies" ) { 
      make_ref_efficiencies = true;
    }
    else if ( arg=="-r" || arg=="--refit" ) { 
      refit_resplots = true;
    }
    else if ( arg=="-rr" || arg=="--refitref" ) { 
      refitref_resplots = true;
    }
    else if ( arg=="--oldrms" ) { 
      oldrms = true;
    }
    else if ( arg=="-nw" || arg=="--nowatermark" ) {
      nowatermark = true;
      Plots::setwatermark(!nowatermark);
    }
    else if ( arg=="--chi2" ) { 
      dochi2 = true;
    }
    else if ( arg=="-ns" || arg=="--nostats" ) { 
      nostats = true;
    }
    else if ( arg=="-nm" || arg=="--nomeans" ) { 
      nomeans = true;
    }
    else if ( arg=="-nt" || arg=="--notitle" ) { 
      notitle = true;
    }
    else if ( arg=="-nr" || arg=="--noref" ) { 
      Plotter::setplotref(false);
      noref = true;
    }
    else if ( arg=="--normref" ) { 
      normref = true;
    }
    else if ( arg=="-rc" || arg=="--refchains" ) { 
      addingrefchains = true;
    }
    else if ( arg=="-uc" || arg=="--usechainref" ) { 
      usechainref = true;
    }
    else if ( arg=="-nb" || arg=="--nobayes" ) { 
      _bayes = false;
    }
    else if ( arg=="-es" || arg=="--effscale" ) { 
      if ( ++i<argc ) scale_eff=std::atof(argv[i]);
      else return usage(argv[0], -1, "no efficiency scale provided");
    } 
    else if ( arg=="-er" || arg=="--effscaleref" ) { 
      if ( ++i<argc ) scale_eff_ref=std::atof(argv[i]);
      else return usage(argv[0], -1, "no efficiency scale for the reference histograms provided");
    } 
    else if (               arg=="--ncols" ) { 
      if ( ++i<argc ) ncols=std::atoi(argv[i]);
      else return usage(argv[0], -1, "no number of columns provided");
    } 
    else if ( arg=="-np" || arg=="--noplots" ) { 
      noplots = true;
    }
    else if ( arg=="-C" || arg=="--Cfiles" ) { 
      Cfile = true;
    }
    else if ( arg=="--deleteref" ) { 
      deleteref = true;
    }
    else if (              arg=="--nopng" ) { 
      nopng = true;
    }
    else if (              arg=="--nopdf" ) { 
      nopdf = true;
    }
    else if ( arg=="-as" || arg=="--atlasstyle" ) { 
      atlasstyle = true;
    }
    else if ( arg=="-q" || arg=="--quiet" ) { 
      quiet = true;
    } 
    else if ( arg=="-al" || arg=="--atlaslabel" ) { 
      if ( ++i<argc ) atlaslabel_tmp=argv[i];
      else return usage(argv[0], -1, "no label provided");
    }
    else if ( arg=="-xo" || arg=="--xoffset" ) { 
      if ( ++i<argc ) xoffset=std::atof(argv[i]);
      else return usage(argv[0], -1, "no xoffset provided");
    }
    else if ( arg=="-yp" || arg=="--ypos" ) { 
      if ( ++i<argc ) _ypos=std::atof(argv[i]);
      else return usage(argv[0], -1, "no y position provided");
    }
    else if ( arg=="-xe" || arg=="--xerror" ) { 
      if ( ++i<argc ) xerror=std::atof(argv[i]);
      else return usage(argv[0], -1, "no x error provided");
    }
    else if ( arg=="-s" || arg=="--swap" ) { 
      if ( ++i<argc ) pattern=argv[i];
      else return usage(argv[0], -1, "no patterns provided");
      if ( ++i<argc ) regex=argv[i];
      else return usage(argv[0], -1, "no target pattern provided");
    }
    else if ( arg.find("-")==0 ) {
      std::cerr << "unknown option: " << arg << "\n" << std::endl;
      return usage(argv[0], -4);
    }
    else { 
      if      ( ftestname=="" ) ftestname = arg;
      else if (  frefname=="" ) frefname = arg;
      else { 
	std::string file = "";
	
	if ( arg.find(":file:")!=std::string::npos ) { 
	  file = arg.substr( 0, arg.find(":file:") );
	  chainfiles.push_back( file );
	  arg  = arg.substr(arg.find(":file:")+6,arg.size());
	}

	std::string chain = arg;

	replace ( chain, ":", "_" );
	replace ( chain, ";", "_" );
	chains.push_back(chain);

	std::cout << "file: " << file << "\tchain: " << chain << std::endl;

      }
    }
  }

  if ( ftestname=="" )  { 
    std::cerr << "main(): test file not specified " << std::endl;
    return -1;
  }

  if ( !exists(ftestname) ) { 
    std::cerr << "main(): test file " << ftestname << " does not exist" << std::endl;
    return -1;
  }

  std::vector<TFile*> chainTFiles;



  if ( chainfiles.size()==0 )  ftest_ = TFile::Open( ftestname.c_str() );
  else {
    noref=true;
    chainTFiles.resize(chainfiles.size());
    for ( size_t i=0 ; i<chainfiles.size() ; i++ ) { 
      chainTFiles[i] = TFile::Open( chainfiles[i].c_str() );
      if ( chainTFiles[i] == 0 ) { 
	std::cerr << "\tfile: " << chainfiles[i] << " could not be opened" << std::endl;
	return -1;
      }
      else std::cout << "\tchainfiles: " << chainfiles[i] << " " << chainTFiles[i] << std::endl;
    }
  }


  if ( noref==false ) { 
    if ( frefname=="" )  { 
      std::cerr << "main(): ref file not specified " << std::endl;
      return -1;
    }
    
    if ( frefname==ftestname )    fref_ = ftest_;
    else if ( exists(frefname) )  fref_ = TFile::Open( frefname.c_str() );
    else { 
      std::cerr << "main(): ref file " << frefname << " does not exist" << std::endl;
      //  return -1;
      Plotter::setplotref(false);
      noref=true;
    }
  }
  else fref_ = ftest_;
  
  if ( chainfiles.size()==0 ) { 

    if ( ftest_==0 || ( noref==false && fref_==0 ) ) { 
      std::cerr << "could not open files " << std::endl;
      return -1;
    }
    
  }
  
  if ( scale_eff     == -1 ) scale_eff     = 100;
  if ( scale_eff_ref == -1 ) scale_eff_ref = scale_eff;
  

  bool noreftmp = noref;

  if ( chains.size()==0 ) return usage(argv[0], -1, "no chains specified");

  if ( refchains.size()>0 && refchains.size()!=chains.size() ) return usage(argv[0], -1, "not enough chains specified");
  
  if ( refchains.size()==0 ) refchains = chains;
  
  std::vector<std::string> chainref(chains.size(),"");
  std::vector<std::string> chain_name(chains.size(),"");

  std::vector<std::string> refchain(chainref.size(),"");
	


  std::cout << argv[0] << " options:" << std::endl;
  std::cout << "\tATLAS style:                 " << ( atlasstyle ? "true" : "false" ) << std::endl; 
  std::cout << "\tBayesian uncertainties:      " << ( _bayes ? "true" : "false" ) << std::endl; 
  std::cout << "\trefit resplot uncertainties: " << ( refit_resplots ? "true" : "false" ) << std::endl; 
  std::cout << "\tsuppress mean and rms stats: " << ( nostats ? "true" : "false" ) << std::endl;  
  if ( !nostats ) std::cout << "\tsuppress meanstats:          " << ( nomeans ? "true" : "false" ) << std::endl;  
  std::cout << "\tsuppress png output:         " << ( nopng ? "true" : "false" ) << std::endl;  
  std::cout << "\tsuppress pdf output:         " << ( nopdf ? "true" : "false" ) << std::endl;  
  std::cout << "\tsuppress reference output:   " << ( noref ? "true" : "false" ) << std::endl;  
  std::cout << "\tuse chain references:        " << ( usechainref ? "true" : "false" ) << std::endl;
  std::cout << "\tpanel ncols:                 " << ncols << std::endl;

  if ( usrlabels.size()>0 )  std::cout << "\tlabels:                      " << usrlabels.size() << std::endl;        
  if ( taglabels.size()>0 )  std::cout << "\textra text:                  " << taglabels << std::endl;  
  

  for ( size_t il=0 ; il<usrlabels.size() ; il++ ) { 
    std::cout << "usr label[" << il << "] : " << usrlabels[il] << std::endl;
  } 

  std::cout << "atlas style : " << atlasstyle << std::endl;

  if ( atlasstyle ) { 
    SetAtlasStyle();
    gStyle = AtlasStyle();
    gStyle->cd();
  }
  else { 
    gROOT->SetStyle("Plain");
    gStyle->cd();
    gStyle->SetLineScalePS(1);
    xoffset += 0.02;
  }

  gStyle->SetErrorX(xerror);

  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);

  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadTopMargin(0.05);

  std::cout << "Chains: " << std::endl;
  for ( unsigned ic=0 ; ic<chains.size() ; ic++ ) std::cout << "\t" << chains[ic] << std::endl;    

  if ( usrlabels.size()>0 ) std::cout << "labels: " << usrlabels << std::endl;

  if ( usrlabels.size()>0 && usrlabels.size()==chains.size() ) uselabels = true;

  /// get release data

  TTree*   dataTree = 0;
  TString* releaseData = new TString("");
  std::vector<std::string> release_data;
  
  if ( !nowatermark ) { 

    dataTree = (TTree*)ftest_->Get("dataTree");
  
    if ( dataTree ) { 
      dataTree->SetBranchAddress( "ReleaseMetaData", &releaseData);
      
      for (unsigned int i=0; i<dataTree->GetEntries() ; i++ ) {
	dataTree->GetEntry(i);      
	release_data.push_back( releaseData->Data() );
	std::cout << "main() release data: " << release_data.back() << " : " << *releaseData << std::endl;
      }
    }
  
  
    if ( release_data.size()>0 ) { 
      if ( release_data.size()>1 ) std::cerr << "main() more than one release - using only the first" << std::endl;  
      
      //    std::cout << "release: " << chop(release_data[0], " " ) << std::endl;
      
      std::string nightly = chop(release_data[0], " " );

      if ( contains(nightly,"private" ) ) { 
	for ( int ic=0 ; ic<4 ; ic++ ) chop(release_data[0], " " );
	release += "  (" + release_data[0]+")"; 
      }
      else {
	release += "  (" + nightly; 
	chop( release_data[0], " " );
	release += " " + chop(release_data[0], " " ) + ")";
      }
    }
  }
  

  // Make output directory                                                                                                                           
  if (dir != "") {
    
    gDirectory->pwd();
    
    std::cout << "trying to make directory" << std::endl;
    dir += "/";
    if ( !quiet && !noplots && !exists(dir) ) { 
      if ( mkdir( dir.c_str(), 0777 ) ) std::cerr << "main() couldn't create directory " << dir << std::endl;
      else                              std::cout << "main() output will be sent to directory " << dir << std::endl; 
    }
  }

  if ( ftest_==0 && chainTFiles.size()>0 ) fref_ = ftest_ = chainTFiles[0]; 

  TFile* ftest = ftest_;
  TFile* fref  = fref_;

  std::string testrun = findrun( ftest );

  std::string  rawrefrun = "";
  std::string  refrun = "";

  if ( fref_ ) refrun = rawrefrun = findrun( fref  );

  std::cout << "testrun: " << testrun << "\nrefrun:  " << refrun << std::endl;

  if ( testrun != "" && refrun != testrun ) { 
    if ( pattern == "" ) { 
      pattern = testrun;
      regex   =  refrun;
    }
  }
  
  if ( refrun != "" ) { 

    std::string newtag = "Reference: ";

    std::cout << "refrun: " << refrun << std::endl;

    size_t pos;
    while ( (pos=refrun.find("_"))!=std::string::npos ) refrun.replace( pos, 1, " " );
    newtag += refrun;

    std::string rawrun = refrun.erase( refrun.find("run"), 4 );

    if ( contains(frefname, rawrun) ) { 
      
      std::string release = frefname;

      release.erase( 0, release.find(rawrun) ); 
   
      if    ( contains(release,"HIST") ) release.erase( 0, release.find("HIST")+5 ); 
      while ( contains(release,".") ) release.erase( release.find("."), release.size() ); 
      while ( contains(release,"-") ) release.erase( release.find("-"), release.size() ); 
      while ( contains(release,"_p") ) release.erase( release.find("_p"), release.size() ); 
      while ( contains(release,"_t") ) release.erase( release.find("_t"), release.size() ); 

      newtag += " ";
      newtag += release;

    } 

    taglabels.push_back( newtag );
    
    std::cout << "tag labels: " << taglabels << std::endl;

  } 
  
  /// we divide the resolutions by these
  /// values if we have an "entries" histogram
  /// to tell us how many events there are
  /// overall
  int NeventTest = 1;
  int NeventRef  = 1;


  std::vector<std::string> savedhistos;

  
  if ( !nowatermark ) { 

    TH1D* htestev = (TH1D*)ftest->Get("event") ;
    TH1D* hrefev  = (TH1D*)fref->Get("event") ;

    std::cout << "htestev " << htestev << " " << hrefev  << std::endl;
    
    if ( htestev ) NeventTest = htestev->GetEntries(); 
    if ( hrefev )  NeventRef  = hrefev->GetEntries();
  
    savedhistos.push_back("event");
  }
  else { 
    NeventTest = 1; /// get from Chains? 
    NeventRef  = 1; 
  }

  chainmap_t* chainmap = nullptr;

  if ( mapfile == "" ) mapfile = configfile;

  if ( mapfile != "" ) {

    ReadCards m( mapfile );

    if ( m.isTagDefined( "ChainMap" ) )  {
      std::vector<std::string> chains = m.GetStringVector( "ChainMap" );
      
      chainmap = new chainmap_t();
      
      for ( size_t i=0 ; i<chains.size() ; i+=2 ) { 
	chainmap->insert( chainmap_t::value_type( chains[i], chains[i+1] ) );
      }
      
      std::cout << "\nusing chain map:" << std::endl;
      
      for ( chainmap_t::iterator itr=chainmap->begin() ; itr!=chainmap->end() ; itr++ ) { 
	std::cout << "\t" << itr->first << "\t" << itr->second << std::endl;
      }
    }
  }

  /// set up the correct reference chain names ...

  std::cout << "\ncreating chain and reference information ..." << std::endl;

  for ( size_t j=0; j<chains.size(); j++)  {
	
    /// get the actual chain name and track collection from 

    /// the Chain histogram if present
    std::cout << "chain:        " << chains[j] << "\taddchains: " << addchains << std::endl;
 
    if ( addchains && ( contains(chains[j],"Shifter") || ( !contains(chains[j],"HLT_") && !contains(chains[j], "Fullscan" ) ) ) ) { 

      TFile* fftest = ftest;
  
      if ( chainfiles.size()>0 && chainfiles.size()>j ) fftest = chainTFiles[j];

      TH1F* hchain = Get( *fftest, chains[j]+"/Chain", testrun );
      
      if ( hchain ) { 
	
	std::string name = hchain->GetTitle();
	
	if ( usechainref && !contains(chains[j],"Purity") ) { 
	
	  chainref[j] = name;
	  
	  std::cout << "new chainref:   " << chainref[j] << std::endl;

	  std::string::size_type pos = chainref[j].find(":for");
	  if ( pos!=std::string::npos ) chainref[j].replace( pos, 4, "_for" );
	  std::replace( chainref[j].begin(), chainref[j].end(), ':', '/');	    
	  std::string newchain = dirname( dirname( chains[j]) ) + "/Expert/" + chainref[j];
	  chainref[j] = newchain;

	  std::cout << "final chainref: " << chainref[j] << std::endl;

	}

	std::cout << "chainref:     " << chainref[j] << std::endl;
	
	while ( contains( name, "HLT_" ) ) name = name.erase( name.find("HLT_"), 4 );

	std::cout << "name:         " << name << std::endl;

	if ( contains( name, ":" ) )  chain_name[j] = name.substr( 0, name.find(":") ) + " : ";
	else                           chain_name[j] = name;

	if  ( chain_name[j] == " : "  )  chain_name[j] = "";
	
	std::cout << "chain_name: " << chain_name[j] << std::endl;
      }
    }
  }


  std::cout << "chainref size: " << chainref.size() << "  " << refchains.size() << std::endl;

  for ( size_t j=0 ; j<chainref.size() ; j++ ) { 
    std::cout << "chainref: " << chainref[j] << " :: " << refchains[j] << std::endl;
    if ( chainref[j]!="" ) refchain[j] = fullreplace( chainref[j], pattern, regex );
    else                   refchain[j] = fullreplace( refchains[j], pattern, regex );
    std::cout << "refchain: " << refchain[j] << std::endl;
  }

  std::cout << "done chains" << std::endl;
  
  /// set up the plot panels 

  std::vector<Panel> panels;

  /// read config in from a file if requested ...

  bands bnd;


  std::vector<int>         ccolours;
  std::vector<int>         cstyles;

  std::vector<std::string> ctags;
  std::vector<std::string> ctaglabels;


  std::cout << "\n" << argv[0] << "\tconfigfile: " << configfile << std::endl;

  bool use_file_config = false;
 
  if ( configfile!="" ) { 

    if ( exists(configfile) ) { 

      std::cout << argv[0] << ":\treading configuration file " << configfile << std::endl; 
    
      ReadCards rc(configfile);


      /// read the histos - 1 panel per histo
      
      if ( rc.isTagDefined( "histos" ) ) { 
	
	std::cout << argv[0] << ":\treading histogram configuration from file " << configfile << std::endl; 

	use_file_config = true;

	std::vector<std::string> raw_input = rc.GetStringVector( "histos" );
	
	for ( size_t iraw=0 ; iraw<raw_input.size() ; iraw += 6) {
	  HistDetails h( &(raw_input[iraw]) );	
	  Panel p( h.name(), 1 ); 
	  p.push_back( h );
	  panels.push_back( p );
	}
	
      }
      
      std::cout << "searching for panels" << std::endl;

      /// read in the panel descriptions

      if ( rc.isTagDefined( "panels" ) ) { 
	
	std::cout << argv[0] << ":\treading histogram panel configuration from file " << configfile << std::endl; 

	use_file_config = true;

	std::vector<std::string> panel_config = rc.GetStringVector( "panels" );
	
	for ( size_t ipanel=panel_config.size() ; ipanel-- ;  ) { 
	  
	  std::vector<std::string> raw_input = rc.GetStringVector( panel_config[ipanel] );
	  
	  Panel p( panel_config[ipanel], ncols );
	  
	  if ( raw_input.empty() ) throw std::exception();
	  for ( size_t iraw=0 ; iraw<raw_input.size() ; iraw += 6 ) p.push_back( HistDetails( &(raw_input[iraw]) ) );
	  	  
	  panels.push_back( p ); 

	}

      }	  
  
      if ( rc.isTagDefined( "Bands" ) && rc.isTagDefined( "Labels" ) ) { 
	bnd = bands( rc.GetVector( "Bands"), rc.GetStringVector( "Labels" ) );
      }


      if ( rc.isTagDefined("Colours") ) ccolours = rc.GetIntVector("Colours");
      if ( rc.isTagDefined("Styles") )  cstyles   = rc.GetIntVector("Styles");
      if ( rc.isTagDefined("Tags") )    ctags     = rc.GetStringVector("Tags");
      if ( rc.isTagDefined("TagLabels") ) ctaglabels = rc.GetStringVector("TagLabels");
      if ( rc.isTagDefined("TagLabels") ) usrlabels = rc.GetStringVector("TagLabels");
    

      uselabels = true;


      std::cout << argv[0] << "\tuserlabels :" << usrlabels << ":" << std::endl;



      if ( rc.isTagDefined("RANGEMAP") )    RANGEMAP = true;
      if ( rc.isTagDefined("ALLRANGEMAP") ) ALLRANGEMAP = true;

      std::cout << "Extra: " << rc.isTagDefined("Extra") << std::endl;
      
      if ( rc.isTagDefined("Extra") ) taglabels.push_back( fullreplace( rc.GetString("Extra"), "__", " " ) );


    }
    else { 
      std::cerr << argv[0] << ":\t config file not found: " << configfile << std::endl;
      return -1;
    }

  }

  if ( !use_file_config ) {

    std::cout << "using default panels" << std::endl;

    /// use the default values as single histogram panels

    // for ( size_t iraw=0 ; iraw<Nhistos ; iraw++ ) { 
    //    Panel p( histos_default[iraw][0], 1 );
    //    p.push_back( histos_default[iraw] );
    //    panels.push_back( p );
    // }

    // default panel efficiencies plotted from 0 to 100, 
    // so scale efficiencies no matter what

    scale_eff     = 100;
    scale_eff_ref = 100;

    /// use the default panels

    std::string (*inpanels[3])[6] = { eff_panel, res_panel, diff_panel };

    size_t nphist[3] = { 4, 4, 10 }; 

    std::string pnames[3] = { "eff", "res", "diff" };

    for ( size_t ip=0 ; ip<3 ; ip++ ) { 
      Panel p( pnames[ip]+"_panel", 2 );
      for ( size_t iraw=0 ; iraw<nphist[ip] ; iraw++ ) p.push_back( HistDetails( inpanels[ip][iraw] ) );
      panels.push_back( p );
    }

  }


  std::cout << "taglabels" << std::endl;

  for ( size_t it=0 ; it<taglabels.size() ; it++ ) std::cout << taglabels[it] << std::endl;


  std::cout << "\npanels: " << panels.size() << std::endl; 

  if ( panels.size()==0 ) return usage(argv[0], -1, "no panels to plot");

  for ( size_t ip=0 ; ip<panels.size() ; ip++ ) std::cout << panels[ip] << std::endl;


  /// a better colour palette than the egregious root default
  if ( true ) { 
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadTopMargin(0.05);

    const Int_t Number = 3;
    Double_t Red[Number] = { 0.00, 0.00, 1.00};
    Double_t Green[Number] = { 0.00, 5.00, 1.00};
    Double_t Blue[Number] = { 0.00, 0.50, 0.00};
    Double_t Length[Number] = { 0.00, 0.50, 1.00 };
    Int_t nb=50;
    TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);
  }
  else gStyle->SetPalette(1);

  if ( fulldbg ) std::cout << __LINE__ << std::endl;

  double rightmargin = gStyle->GetPadRightMargin(); 
  gStyle->SetPadRightMargin(0.1); 

  /// Fixme: add the 2D histogram processing here ??

  gStyle->SetPadRightMargin(rightmargin); 
    
  // gStyle->SetLineScalePS(1);

  for ( size_t ipanel=0 ; ipanel<panels.size() ; ipanel++ ) { 

    Panel& panel = panels[ipanel]; 

    std::cout << "\n\n---------------------------------------------\n";

    std::cout << panel << "\n" << std::endl;

    int ncolsp = panel.ncols();
    int nrowsp = panel.nrows();
    
    double extraw = 1;

    std::cout << "\nncols: " << ncolsp << "\tnrows: " << nrowsp << std::endl;

    bool multipanel = ( panel.size() > 1 );

    if ( panel.size()==0 ) { 
      std::cout << "panel empty: " << panel.name() << std::endl;
      continue;
    }

    gStyle->cd();
    
    if ( panel.size()>4 ) gStyle->SetLineScalePS(0.5);
 
    if ( multipanel ) extraw = 1.05;

    TCanvas* tc = new TCanvas( "tc", "", extraw*ncolsp*800, nrowsp*600 );

    tc->cd();

    std::string atlaslabel = atlaslabel_tmp;

    
    if ( multipanel ) { 

      gStyle->SetLineScalePS(1);

      /// words can not express the sheer unpleasantness in trying to 
      /// do anything serious with subpanels

      /// - if any margins between pads are set to 0, root messes 
      ///   up the *pad* margins so they are 0 with seemingly no way 
      ///   to prevent it - have to use a very small (but non 0) value, 
      ///   but then the margins don't size properly. 
      /// - Once you cd() to one of the child pads, if you 
      ///   ask for the child GetWh() or GetWw() you *don't* get the 
      ///   parameters of the child pad, but the *main* pad etc

      /// How are you supposed to get the paremeters of the *actual* 
      /// pad ? It is insane.
      
      tc->Divide( ncolsp, nrowsp, 0.0001, 0.0003 );
      //  atlaslabel = "     " + atlaslabel_tmp;
    }

    /// histos within the panel

    std::string plotname = ""; 

    for ( size_t i=0 ; i<panel.size() ; i++ ) {
      
      HistDetails histo = panel[i];

      std::string xaxis = histo.xtitle();
      std::string yaxis = histo.ytitle();
      
      AxisInfo xinfo = histo.xaxis(); 
      AxisInfo yinfo = histo.yaxis(); 

      int npanel = nrowsp*(i/nrowsp) + i%nrowsp + 1 ;
      
      std::cout << "panel: panel: " << panel.name() << "\tsubpanel: " << npanel << std::endl;

      if ( multipanel ) tc->cd( npanel );

      SetAtlasStyle();

      noreftmp = noref;

      Plotter::setplotref(!noreftmp);
      
      if ( fulldbg )  std::cout << __LINE__ << std::endl;
      
      std::cout << "main() processing histo[" << i <<  "] " << (i<10 ? " " : "" ) << histo.name() << "\t" << histo.xaxis() << std::endl;
      
      Plots plots_eff( "", yinfo.trim() );
      plots_eff.clear();
      
      Plots plots( "", yinfo.trim() );
      plots.clear();
      
      
            
      double xpos  = 0.18;
      double ypos  = 0.93;
      
      if ( contains(histo.name(),"eff") || contains(histo.name(),"Eff_") ) ypos = 0.15;

      if ( histo.name()=="pT" || histo.name()=="pT_rec" ) ypos = 0.15;
      
      if ( atlasstyle ) { 
	xpos  = 0.18;
	if ( ypos>0.5 ) ypos = 0.85; 
	else            ypos = 0.18;
      }
      
      if ( _ypos!=0 ) ypos = _ypos;
      
      double xpos_original = xpos;

      xpos += xoffset;
      
      if ( xinfo.offset() != 0 ) { 

	std::cout << "HA ! xinfo.offset:  " << xinfo.offset() << std::endl; 

	xpos = xinfo.offset(); 

      }
      

      if ( yinfo.offset() != 0 ) { 

	std::cout << "HA ! yinfo.offset:  " << yinfo.offset() << std::endl; 

	ypos = yinfo.offset(); 

      }
      

      
      /// calculate all the postions for the items in the legend

      size_t Nrows = chains.size();

      if ( ALLRANGEMAP || (RANGEMAP && xaxis.find("p_{T}")!=std::string::npos && ccolours.size() ) ) 
	Nrows = ( Nrows < ccolours.size() ? Nrows : ccolours.size() );
      
      std::cout << "\n\n\tNrows " << Nrows << std::endl;

      int Nlines = Nrows + taglabels.size();
      
      std::vector<double> ypositions;
      
      double deltay = (Nrows*0.055-0.005)/Nrows;
      
      double ylo = ypos;
      double yhi = ypos;
      
      if ( ypos>0.5 ) ylo -= Nlines*deltay;
      else            yhi += Nlines*deltay;
      
      for ( int ilines=0 ; ilines<Nlines ; ilines++ ) { 
	ypositions.push_back( yhi - deltay*(ilines+0.5) );
      }

      // specify different legends for efficiencies or residuals?
      
      /// legends ....

      Legend legend( xpos, xpos+0.1, ylo, ylo+Nrows*0.06-0.005 );
      Legend legend_eff( xpos, xpos+0.1, ylo, ylo+Nrows*0.06-0.005 );
            
      
      std::vector<std::string> Mean;
      std::vector<std::string> RMS;
      
      Mean.clear();
      RMS.clear();
      
      std::vector<std::string> Chi2;
      std::vector<std::string> MeanRef;
      std::vector<std::string> RMSRef;
      
      //    int colours[6] = { 1, 2, 4, 6, 7, 8 };
      
      Chi2.clear();
      MeanRef.clear();
      RMSRef.clear();
      
      int mean_power = 0;
      int rms_power  = 0;
      bool power_set = false;
      
      LINEF = true;
      
      ///  leave these comments here for testing ...
      //    bool uselogx = xinfo.log();
      //    bool uselogy = yinfo.log();
      
      for ( unsigned int j=0; j<chains.size(); j++)  {
	
      TFile* fftest = ftest;
      TFile* ffref  = fref;

      if ( chainfiles.size()>0 && chainfiles.size()>j ) ffref = fftest = chainTFiles[j];

	/// get the actual chain name and track collection from 
	/// the Chain histogram if present
	
	std::cout << "chain:    " << chains[j] << "\taddchains: " << addchains << std::endl;
	std::cout << "chainref: " << chainref[j] << std::endl;

	noreftmp = noref;
	Plotter::setplotref(!noreftmp);
	
	TH1F* htest = 0;
	TH1F* href  = 0;
	
	TH1F* htestnum = 0;
	TH1F* htestden = 0;
	TH1F* hrefnum  = 0;
	
	TGraphAsymmErrors* tgtest = 0;
	
	std::cout << "refchain.size() " << refchain.size() << std::endl;
	
	std::cout << "refchain: " << refchain[j] << std::endl;
	
	/// refit the resplots - get the 2d histogram and refit
	
	if ( refit_resplots && ( contains(histo.name(),"/sigma") || contains(histo.name(),"/mean") ) ) { 
	  
	  bool bsigma = false;
	  if ( contains(histo.name(),"/sigma") ) bsigma = true;

	  bool bmean = false;
	  if ( contains(histo.name(),"/mean") ) bmean = true;

	  std::cout << "\trefitting:  " << histo.name() << std::endl;
	    
	  Resplot::setoldrms95(oldrms);
	  Resplot::setscalerms95(true);
	    
	  std::string tmp_  = histo.name();
	  std::string base;
	    
	  if ( bsigma ) base = chop( tmp_, "/sigma" );
	  if ( bmean )  base = chop( tmp_, "/mean" );
	    
	  TH2D* htest2d_ = Get<TH2D>( *fftest, chains[j]+"/"+base+"/2d", testrun, 0, &savedhistos );
	  
	  std::cout << "ffref " << ffref << std::endl;
	  TH2D* href2d_  = 0;

	  if ( ffref ) href2d_ = Get<TH2D>( *ffref,  chains[j]+"/"+base+"/2d", testrun  );

	  if ( htest2d_==0 ) continue;
	  if ( !noreftmp && href2d_==0 )  noreftmp = true;
	  Plotter::setplotref(!noreftmp);
	    
	  /// get the test histogram
	    
	  Resplot rtest("tmp", htest2d_ );

	  if ( rtest.finalised() ) { 
	    if ( contains(histo.name(),"npix") || contains(histo.name(),"nsct") || contains(histo.name(),"nsi") || contains(histo.name(),"nbl") ) rtest.Refit(Resplot::FitNull);
	    else  rtest.Refit(Resplot::FitNull95);
	  }
	  else {
	    if ( contains(histo.name(),"npix") || contains(histo.name(),"nsct") || contains(histo.name(),"nsi") || contains(histo.name(),"nbl") ) rtest.Finalise(Resplot::FitNull);
	    else  rtest.Finalise(Resplot::FitNull95);
	  }

	  if ( bsigma ) { htest = (TH1F*)rtest.Sigma()->Clone("rtest_sigma"); htest->SetDirectory(0); }
	  if ( bmean  ) { htest = (TH1F*)rtest.Mean()->Clone("rtest_mean");   htest->SetDirectory(0); }

	  if ( htest==0 ) { 
	    std::cerr << "missing test histogram: " << (refchain[j]+" / "+histo.name()) << " " << htest << "(test)" << std::endl; 
	    continue;
	  }


	  /// Actually DO refit the references - if we are adding together many
	  /// resplots, then the means etc may not even have been calculated

	  TH1F* hreft  = 0;

	  if ( !noreftmp ) {
	    if ( refitref_resplots ) { 

	      Resplot rref("tmp", href2d_ );
		
	      if ( rref.finalised() ) { 
		if ( contains(histo.name(),"npix") || contains(histo.name(),"nsct") ) rref.Refit(Resplot::FitNull);
		else  rref.Refit(Resplot::FitNull95);
	      }
	      else {
		if ( contains(histo.name(),"npix") || contains(histo.name(),"nsct") ) rref.Finalise(Resplot::FitNull);
		else  rref.Finalise(Resplot::FitNull95);
	      }
		
	      if ( bsigma ) { hreft = (TH1F*)rref.Sigma()->Clone("rref_sigma"); hreft->SetDirectory(0); }
	      if ( bmean )  { hreft = (TH1F*)rref.Mean()->Clone("rref_mean"); hreft->SetDirectory(0); }
		
	    }
	    else { 
	      hreft = Get( *ffref, refchain[j]+"/"+histo.name(), rawrefrun, chainmap );
	    }
	  }
	      
	  if ( !noreftmp && hreft==0 ) { 
	    std::cerr << "missing ref histogram: " << (refchain[j]+" / "+histo.name()) << " " << htest << "(ref)" << std::endl; 
	    noreftmp = true;
	    std::exit(-1);
	    Plotter::setplotref(!noreftmp);
	  }
	      
	  if ( !noreftmp ) { 
	    href = (TH1F*)hreft->Clone();
	    href->SetDirectory(0);
	  }
	    	      
	  /// useful test for debugging ...
	  //	    std::cout << "\tget " << (refchain[j]+"/"+histos[i]) << "\t" << href << std::endl;
	    
	  savedhistos.push_back( refchain[j]+"/"+histo.name() );
   	   
	}
	else { 

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	  /// everything else 
       	
	  std::string reghist = histo.name();

	  std::cout << "hist:  " << (chains[j]+"/"+reghist) << "\tftest " << ftest << std::endl;

	  htest = Get( *fftest, chains[j]+"/"+reghist, testrun, 0, &savedhistos );

	  std::cout << "hist: " << htest << std::endl;

	  std::cout << xaxis << std::endl;

	  if ( htest==0 ) { 
	    if ( htest==0 ) std::cerr << "missing test histogram: " << (chains[j]+" / "+reghist) << " " << htest<< std::endl; 
	    continue;
	  }

	  TH1F* hreft = 0;

	  std::cout << "hreft: " << hreft << std::endl;

	  if ( ffref ) hreft = Get( *ffref, refchain[j]+"/"+reghist, rawrefrun, chainmap );
	  else noreftmp = true;

	  std::cout << "hreft: " << hreft << std::endl;

	  if ( std::string(htest->ClassName()).find("TH2")!=std::string::npos ) { 
	    std::cout << "Class TH2: " << htest->GetName() << std::endl;
	    continue;
	  }

	  if ( std::string(htest->ClassName()).find("TH1")!=std::string::npos ) { 
	    std::cout << "Class TH1: " << htest->GetName() << std::endl; 
	  }
	  else if ( std::string(htest->ClassName()).find("TProfile")!=std::string::npos ) {  
	    std::cout << "Class TProf: " << htest->GetName() << std::endl; 
	  }



	  if ( !noreftmp && hreft==0 ) { 
	    if ( hreft==0 ) std::cerr << "missing ref histogram: " << (refchain[j]+" / "+reghist)  
				      << " " << hreft << std::endl; 
	    noreftmp = true;
	    std::exit(-1);
	    Plotter::setplotref(false);
	  }
	
	  if ( hreft!=0 ) { 
	    href = (TH1F*)hreft->Clone();
	    href->SetDirectory(0);
	  }

	  std::cout << " \tget " << (chains[j]+"/"+reghist)    << "\thtest " << htest << std::endl;
	  std::cout << " \tget " << (refchain[j]+"/"+reghist)  << "\thref  " << href << std::endl;

	  if ( htest==0 || ( !noreftmp && href==0 ) ) continue;


	  if ( fulldbg ) std::cout << __LINE__ << std::endl;
	
	  if ( histo.name().find("rdz_vs_zed")==std::string::npos && histo.name().find("1d")!=std::string::npos ) { 
	    std::cout << "Rebinning histogram: " << histo.name() << std::endl;
	    if (        htest->GetNbinsX()>500 ) htest->Rebin(10);
	    if ( href && href->GetNbinsX()>500 ) href->Rebin(10);
	  }


	  if ( histo.name().find("zed_eff")!=std::string::npos ) { 
	    if (        htest->GetNbinsX()>100 ) htest->Rebin(5);
	    if ( href && href->GetNbinsX()>100 ) href->Rebin(5);
	  }


	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	  if ( scalepix && std::string(htest->GetName()).find("npix")!=std::string::npos ) htest->Scale(0.5);
	  if ( scalepix && href && std::string(htest->GetName()).find("npix")!=std::string::npos ) href->Scale(0.5);

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	  if ( notitle ) { 
	    htest->SetTitle("");
	    if( href ) href->SetTitle("");
	  } 

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	}

	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	if ( make_ref_efficiencies ) { 
	  if ( htest && href ) { 

	    /// debugging ...
	    //	  std::cout << "contains _eff " << contains( std::string(htest->GetName()), "eff" ) << std::endl;

	    if ( contains( std::string(htest->GetName()), "eff" ) ) {

	      std::string effhist = histo.name();
  
	      htestnum = Get( *fftest, chains[j]+"/"+effhist+"_n", testrun, 0, &savedhistos );

	      TH1F* hrefnumt = Get( *ffref, refchain[j]+"/"+effhist+"_n", rawrefrun, chainmap, &savedhistos );

	      if ( !noreftmp && hrefnumt!=0 ) { 
		hrefnum = (TH1F*)hrefnumt->Clone();
		hrefnum->SetDirectory(0);
	      }

	    }
	  }
	}

	
	if ( _bayes ) { 

	  if ( htest && contains( std::string(htest->GetName()), "eff" ) ) {

	    double rebin = 0;
	    
	    std::string effhist = histo.name();
	    
	    /// rebin if requested ...
	    
	    if ( contains( effhist, "+Rebin" ) ) { 
	        rebin = std::atof( effhist.substr( effhist.find("+Rebin")+6, effhist.size() ).c_str() ); 
		effhist = effhist.substr( 0, effhist.find("+Rebin") ); 
	    } 

	    if ( contains( effhist, "+rebin" ) ) { 
	        rebin = std::atof( effhist.substr( effhist.find("+rebin")+6, effhist.size() ).c_str() ); 
		effhist = effhist.substr( 0, effhist.find("+rebin") ); 
	    } 
	      
	    htestnum = Get( *fftest, chains[j]+"/"+effhist+"_n", testrun, 0, &savedhistos ) ;
	    htestden = Get( *fftest, chains[j]+"/"+effhist+"_d", testrun, 0, &savedhistos ) ;

	    if ( rebin!=0 ) { 
    	      htestnum = Rebin(htestnum, rebin );
	      htestden = Rebin(htestden, rebin );
	    }

	    std::cout << "test histogram name: : " << htestnum->GetName() << "\txaxis: " << xaxis << "\t" << std::endl;

	    //	    if ( xaxis.find("p_{T}")!=std::string::npos || xaxis.find("pt")!=std::string::npos ) { 
	    //    b.range( chains[j], htestnum );
	    //   b.range( chains[j], htestden );
	    //  }

	    std::cout << "1: Bayesian error calculation " << htestnum << " " << htestden << "\tscale " << scale_eff << std::endl;

	    if ( htestnum && htestden ) { 

	      if ( std::string(htestnum->GetName()).find("ntrax_eff")!=std::string::npos ) {

		bool low = true;

		//		if ( chains[j].find("j55")!=std::string::npos ) low = false;

		htestnum = rebin_log( htestnum, low );
		htestden = rebin_log( htestden, low );
	      }

#if 0
	      /// need this for lumi block histograms - although not always needed, so 
	      /// excluded to prevent problems most of the time ...
	      if ( contains( htest->GetName(), "_vs_lb" )  ) { 
		std::cout << "rebin " << histo.name() << std::endl;
		htestnum->Rebin(3);
		htestden->Rebin(3);
	      }

	      if ( contains( htest->GetName(), "eta_eff" )  ) { 
		std::cout << "rebin " << histo.name() << std::endl;
		htestnum->Rebin(2);
		htestden->Rebin(2);
	      }
#endif	

	      //    if ( RANGEMAP && (effhist.find("pT")!=std::string::npos || effhist.find("pt")!=std::string::npos ) ) { 
	      if ( RANGEMAP && (effhist.find("ET")!=std::string::npos ) ) { 
		std::cout << "\trange: " << j << " " << htest << std::endl;
		bnd.range( chains[j], htestnum );
		bnd.range( chains[j], htestden );
	      }

    
	      Efficiency e( htestnum, htestden, "", scale_eff );
	      tgtest = e.Bayes(scale_eff);

	      htest = e.Hist();

	      std::cout << "effhist: " << effhist << std::endl;

	    }
	
	    /// now recalculate reference

	    std::cout << "recalculating reference efficiencies ..." << std::endl; 

	    if ( href ) { 

	      std::cout << "doin ..." << std::endl; 

	      TH1F* hrefnum = Get( *ffref, refchain[j]+"/"+histo.name()+"_n", rawrefrun, chainmap );

	      TH1F* hrefden = Get( *ffref, refchain[j]+"/"+histo.name()+"_d", rawrefrun, chainmap );

	      std::cout << "2. Bayesian error calculation " << htestnum << " " << htestden << "\tscale " << scale_eff     << std::endl;
	      std::cout << "3. Bayesian error calculation " << hrefnum  << " " << hrefden  << "\tscale " << scale_eff_ref << std::endl;
	    
	      if ( hrefnum && hrefden ) { 
		Efficiency e( hrefnum, hrefden, "", scale_eff_ref );
		/// leave for documentation purposes ...
		// tgref = e.Bayes(scale_eff);

		href = e.Hist();
	      
	      }
	    }
	  }
	
	}

      
	if ( htest==0 ) { 
	  std::cout << "       no test histogram :   " << (chains[j]+"/"+histo.name()) << std::endl;
	  continue;
	}

	if ( !noreftmp && href==0 ) { 
	  std::cout << "       no ref histogram :    " << (chains[j]+"/"+histo.name()) << std::endl;
	  continue;
	}


	htest->GetYaxis()->SetTitleOffset(1.55); 
	htest->GetXaxis()->SetTitleOffset(1.5); 
	htest->GetXaxis()->SetTitle(xaxis.c_str());
	htest->GetYaxis()->SetTitle(yaxis.c_str());

	if ( !noreftmp ) { 
	  href->GetYaxis()->SetTitleOffset(1.5);
	  href->GetXaxis()->SetTitleOffset(1.5);
	  href->GetXaxis()->SetTitle(xaxis.c_str());
	  if ( contains(yaxis,"Efficiency") && !contains(yaxis,"%") && scale_eff==100 ) href->GetYaxis()->SetTitle((yaxis+" [%]").c_str());
	  else href->GetYaxis()->SetTitle(yaxis.c_str());
	}	

	if ( fulldbg ) std::cout << __LINE__ << std::endl;

#if 1
	if ( contains(histo.name(),"ntracks") ) {

	  double xm  = htest->GetMean();

	  if ( xm>=10 ) { 
	    double lxm = std::log10(xm);
	    int newbins = int(0.5+xm/std::pow(10,int(lxm)))*pow(10,int(lxm));
	    int nrebin   = int( (newbins+5)/10 );
	    
	    if ( nrebin>1 ) { 
	      std::cout << "rebin: " << htest->GetName() << "\tbins: " << nrebin << std::endl;
	      htest->Rebin(nrebin);
	      htest->Sumw2();
	      if ( !noreftmp ) { 
		href->Rebin(nrebin);
	      href->Sumw2();
	      }
	    }
	  }
	}
#endif

	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	/// only set the plot name once )  
	if ( plotname == "" ) { 

	  if ( key!="" ) { 
	    htest->SetTitle("");
	    if ( href ) href->SetTitle("");
	    plotname = key+"_";
	  }
	  else if (fcontains(chains[j],"HLT_")) {
	    htest->SetTitle("");
	    if ( href ) href->SetTitle("");
	    plotname = "HLT_";
	  }
	  else if (fcontains(chains[j],"EF_")) {
	    htest->SetTitle("");
	    if ( href ) href->SetTitle("");
	    plotname = "EF_";
	  }
	  else if (fcontains(chains[j],"L2_")) {
	    htest->SetTitle("");
	    if ( href ) href->SetTitle("");
	    plotname = "L2_";
	  }
	  else if (contains(chains[j],"FTK") && ! contains(chains[j],"HLT_") ) { 
	    htest->SetTitle(("FTK "+ histo.name()).c_str());
	    if ( href ) href->SetTitle(("FTK "+ histo.name()).c_str());
	    plotname = "FTK_";
	  }
	
	  plotname += histo.name(); 

	  /// replace the "/" in the filename so we don't try to 
	  /// make plots in subdirectories by accident  
	  replace(plotname, "/", "_"); 

	}


	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	bool residual = false;
      
	if ( contains(histo.name(),"_res") ||  contains(histo.name(),"residual_") || contains(histo.name(),"1d") ) residual = true; 




	std::string collection = basename( chains[j] );

	std::string actual_chain = basename( dirname( chains[j] ) );

	if ( collection.find("_InDet")!=std::string::npos ) collection.erase( 0, collection.find("_InDet")+1 );
	if ( actual_chain.find("_InDet")!=std::string::npos ) actual_chain.erase( actual_chain.find("_InDet") );
	

	std::cout << "raw:              " << chains[j]    << std::endl;

	std::cout << "track collection: " << collection   << std::endl;
	std::cout << "actual chain:     " << actual_chain << std::endl;


	if ( actual_chain.find("HLT_IDTrack_")!=std::string::npos )    actual_chain.erase( actual_chain.find("HLT_IDTrack_"), 12 );
	if ( actual_chain.find("_idperf")!=std::string::npos )    actual_chain.erase( actual_chain.find("_idperf"), 7 );
	if ( actual_chain.find("_bperf")!=std::string::npos )     actual_chain.erase( actual_chain.find("_bperf"), 6 );
	if ( actual_chain.find("_boffperf")!=std::string::npos )  actual_chain.erase( actual_chain.find("_boffperf"), 9 );
	if ( actual_chain.find("_HLT_")!=std::string::npos )      actual_chain.replace( actual_chain.find("_HLT_"), 5, " " );
	if ( actual_chain.find("HLT_")!=std::string::npos )       actual_chain.erase( actual_chain.find("HLT_"), 4 );
	if ( collection.find("_IDTrkNoCut")!=std::string::npos )  collection.erase( collection.find("_IDTrkNoCut"), 11 );
	if ( collection.find("xAODCnv")!=std::string::npos )      collection.erase( collection.find("xAODCnv"), 7 );
	if ( collection.find("HLT_IDTrack_")!=std::string::npos ) collection.erase( collection.find("HLT_IDTrack_"), 12 );
	if ( collection.find("HLT_IDTrack")!=std::string::npos )  collection.erase( collection.find("HLT_IDTrack"), 11 );
	if ( collection.find("Tracking")!=std::string::npos )     collection.replace( collection.find("Tracking"), 8, "Trk" );    
	if ( collection.find("InDetTrigTrk_")!=std::string::npos ) collection.erase( collection.find("InDetTrigTrk_"), 13 );    
	if ( collection.find("HLT_xAODTracks_")!=std::string::npos ) collection.erase( collection.find("HLT_xAODTracks_"), 15 );    
	if ( collection.find("_HLT_")!=std::string::npos ) collection.replace( collection.find("_HLT_"), 5, " " );    
	if ( collection.find("HLT_")!=std::string::npos )  collection.erase( collection.find("HLT_"), 4 );    

	std::string c = actual_chain + " : " + collection;

	std::cout << "track collection: " << collection   << "   <-" << std::endl;
	std::cout << "actual chain:     " << actual_chain << "   <-" << std::endl;
       
	replace( c, "_In", " :  " );

	c = "  " + c;

	std::cout << "use label: " << c << "\tchains size " << chains.size() << "\t" << usrlabels.size() << std::endl;

	std::cout << "chains.size() " << chains.size() << "  " << j <<  std::endl; 
	std::cout << "chains.size() " << chains.size() << "  " << j <<  std::endl; 

	std::cout << "chains[j] : " << j << " " << chains[j] << std::endl; 

	std::cout << "userlabels.size() " << usrlabels.size() << std::endl; 

	if ( usrlabels.size() < j+1 ) std::cerr << "userlabels not large enough - not using userlabels" << std::endl;
	else c = usrlabels[ j ]; 

	std::cout << "use label: c: " << c << std::endl;

	/// calculate and set axis limits

	//      std::cout << "adding plot " << histos[i] << " " << htest->GetName() << std::endl;

	if ( fulldbg ) std::cout << __LINE__ << std::endl;

      
      
	std::cout << "\n\n\n\nxaxis: " << xaxis << std::endl;

	

	if ( ALLRANGEMAP || xaxis.find("p_{T}")!=std::string::npos || xaxis.find("E_{T}")!=std::string::npos ) {

	  if ( RANGEMAP && xaxis.find("p_{T}")!=std::string::npos ) {
	    bnd.range( chains[j], htest );
	    if ( href ) bnd.range( chains[j], href );
	  }

          if ( RANGEMAP || ALLRANGEMAP ) { 

	    LINEF = false;
	  
	    std::cout << "\n\n\nctags " << ctags.size() << "\n\n" << std::endl;

	    for ( size_t ic=0 ; ic<ctags.size() ; ic++ ) { 
	      
	      //	    std::cout << "\tctags[" << ic << "] =  " << ctags[ic] << std::endl; 
	      
	      std::cout << "\n\nic: " << ic << " " << ctags[ic] << " " << ccolours[ic] << "\n\n" << std::endl;

	      if ( chains[j].find(ctags[ic])!=std::string::npos ) {
		std::cout << "\ttag " << ctags[ic] << "  \tcolour: " << ccolours[ic] << "\tstyle: " << cstyles[ic] << std::endl;
		htest->SetLineColor( ccolours[ic] );
		htest->SetMarkerColor( ccolours[ic] );
		htest->SetMarkerStyle( cstyles[ic] );
		c = ctaglabels[ic];
		if ( tgtest ) { 
		  tgtest->SetLineColor(htest->GetMarkerColor());
		  tgtest->SetMarkerStyle(htest->GetMarkerStyle());
		  tgtest->SetMarkerColor(htest->GetMarkerColor());
		}
		break;
	      }
	    }

	  }

	  std::cout << "test: " << chains[j] << "chains colour: " << htest->GetMarkerColor() << std::endl;

	} 
	
     
	  

	std::cout << "movin' on ..." << std::endl;

	std::cout << "chain: " << chains[j] << " \t marker colour: " << htest->GetMarkerColor() << std::endl;

	//	std::exit(0);
	
	std::cout << "Plotter marker : " << htest->GetMarkerColor() << " " << htest->GetMarkerStyle() << std::endl;

	//      if ( uselabels )  plots.push_back( Plotter( htest, href, chain_name+usrlabels[j], tgtest ) );
	if ( uselabels )  plots.push_back( Plotter( htest, href, " " + chain_name[j] + c, tgtest ) );
	else {
	  std::cout << "using label: " << c << std::endl;
	  plots.push_back( Plotter( htest, href, c, tgtest ) );
	}

	if ( ALLRANGEMAP || ( RANGEMAP && xaxis.find("p_{T}")!=std::string::npos )  ) plots.back().max_entries( ccolours.size() );
	
	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	if ( make_ref_efficiencies ) { 
	
	  if ( htestnum && hrefnum ) { 
	    Efficiency e( htestnum, hrefnum, "", scale_eff );

	    TH1* h = e.Hist();

	    double range = h->GetMaximum()-h->GetMinimum();

	    if ( range<0.2*scale_eff ) {
 
	      double _max = int( (h->GetMaximum() + 20)*0.1 )*0.1*scale_eff;
	      double _min = int( (h->GetMinimum() - 10)*0.1 )*0.1*scale_eff;
	    
	      if ( _max>1*scale_eff ) _max = 1.02*scale_eff;
	      if ( _min<0 )           _min = 0;
	    
	      h->SetMinimum(_min);
	      h->SetMaximum(_max);
	   
	    }

	    plots_eff.push_back( Plotter( e.Hist(), 0, c ) );
	  
	  }     
	}
      

	if ( href ) Chi2.push_back( label( "chi2 = %5.2lf / %2.0lf", chi2( htest, href ), double(htest->GetNbinsX()) ) );

	if ( residual ) {
	
	  /// resolutions 

	  xpos = xpos_original;

	  std::cout << "calculating resolutions : " << histo.name() << " " << htest->GetName() << std::endl;

	  TF1* d95 = Resplot::FitNull95( (TH1D*)htest );
	
	  double   mean_95 = d95->GetParameter(1);
	  double  dmean_95 = d95->GetParError(1);
	  double    rms_95 = d95->GetParameter(2);
	  double   drms_95 = d95->GetParError(2);
	
	  std::cout <<  "\t\t" << histo.name()
		    << "\tmean:     " << mean_95 << " +- " << dmean_95 
		    << "\trms:     "  <<  rms_95 << " +- " << drms_95 << std::endl; 

	  /// calculate power

	  if ( !power_set ) { 
	    for ( int ip=-2 ; ip<9 ; ip++ ) { 
	      if ( std::fabs(mean_95) >= std::pow( 10., double(-ip) ) ) { 
		mean_power = ip;
		break;
	      }
	    }
	  
	    for ( int ip=-2 ; ip<9 ; ip++ ) { 
	      if ( std::fabs(rms_95)  >= std::pow( 10., double(-ip) ) ) { 
		rms_power = ip;
		break;
	      }
	    }
	  }

	  power_set = true;

	  std::cout <<  "\t\t" << histo.name() 
		    << "\tmean:     " << mean_95 << " +- " << dmean_95 << " : pow " << mean_power
		    << "\trms:     "  <<  rms_95 << " +- " << drms_95  << " : pow " << rms_power << std::endl; 


	  if ( mean_power == 0 ) { 
	    Mean.push_back(label("mean_{95}  = %4.2lf #pm %4.2lf", mean_95, dmean_95) );
	  }
	  else { 
	    Mean.push_back(label("mean_{95}  = ( %4.2lf #pm %4.2lf ) #times 10^{%d}", 
				 mean_95*std::pow(10.,double(mean_power)), dmean_95*std::pow(10,double(mean_power)), -mean_power ) );
	  
	  }

	  if ( rms_power == 0 ) { 
	    RMS.push_back(label( "rms_{95}   = %4.2lf #pm %4.2lf", rms_95,  drms_95 ) ); 
	  }
	  else { 
	    RMS.push_back(label( "rms_{95}   = ( %4.2lf #pm %4.2lf ) #times 10^{%d}", 
				 rms_95*std::pow(10.,double(rms_power)),  drms_95*std::pow(10,double(rms_power)), -rms_power ) );
	  }

	  if ( href ) { 
	    TF1* d95ref = Resplot::FitNull95( (TH1D*)href );
	  
	    double   mean_95ref = d95ref->GetParameter(1);
	    double  dmean_95ref = d95ref->GetParError(1);
	    double    rms_95ref = d95ref->GetParameter(2);
	    double   drms_95ref = d95ref->GetParError(2);
	  
	    std::cout <<  "\t\t" << histo.name()
		      << "\tmean ref: " << mean_95ref << " +- " << dmean_95ref << " : pow " << mean_power
		      << "\trms ref: "  <<  rms_95ref << " +- " << drms_95ref  << " : pow " << rms_power << std::endl;
	  
	    if ( mean_power == 0 ) { 
	      MeanRef.push_back(label("mean_{95} ref = %4.2lf #pm %4.2lf", mean_95ref, dmean_95ref) );
	    }
	    else { 
	      MeanRef.push_back(label("mean_{95} ref = ( %4.2lf #pm %4.2lf ) #times 10^{%d}", 
				      mean_95ref*std::pow(10,double(mean_power)), dmean_95ref*std::pow(10,double(mean_power)), -mean_power ) );
	    
	    }
	  
	  
	    if ( rms_power == 0 ) { 
	      RMSRef.push_back(label( "rms_{95}  ref = %4.2lf #pm %4.2lf", rms_95ref,  drms_95ref ) ); 
	    }
	    else { 
	      RMSRef.push_back(label( "rms_{95}  ref = ( %4.2lf #pm %4.2lf ) #times 10^{%d}", 
				      rms_95ref*std::pow(10,double(rms_power)),  drms_95ref*std::pow(10,double(rms_power)), -rms_power ) );
	    }
	  }
	
	  htest->Sumw2();
	  if ( href ) href->Sumw2();
	  htest->Scale(1./NeventTest);
	  if ( href ) href->Scale(1./NeventRef);

	}
     
	if ( yinfo.normset() ) { 
	  Norm( htest );
	  if ( href ) Norm( href );
	}
	else if ( yinfo.refnormset() ) { 
	  if ( href ) Norm( href, Entries(htest) );
	}

	if ( yinfo.binwidth() ) { 
	  binwidth( htest );
	  if ( href ) binwidth( href );
	}
    


	if ( !noreftmp && normref && 
	     !contains( histo.name(), "mean") && !contains( histo.name(), "sigma" ) && 
	     !contains( histo.name(), "Eff")  && !contains( histo.name(), "eff") &&
	     !contains( histo.name(), "Res")  && !contains( histo.name(), "vs") && !contains( histo.name(), "_lb") ) { 
	  Norm( href, Entries( htest ) );
	}

      }

      if ( fulldbg ) std::cout << __LINE__ << std::endl;

      if ( !noplots ) { 

	/// try to localise all axis range setting, log, lin scales etc
	/// to this one place 

	if ( fulldbg ) std::cout << __LINE__ << std::endl;
    
	/// sort out the range settings for the xaxis ...
	plots.sortx( xinfo );

	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	double  yminset = 0;
	double  ymaxset = 0;

	double rmin = 0;
	double rmax = 0;
      
	if ( xinfo.rangeset() ) { 
	  rmin = plots.realmin( plots.lo(), plots.hi() );
	  rmax = plots.realmax( plots.lo(), plots.hi() );
	}
	else {
	  rmin = plots.realmin();
	  rmax = plots.realmax();
	}
      	
	if ( yinfo.autoset() ) { 
	
	  int csize = chains.size() + taglabels.size() + ( atlasstyle ? 1 : 0 );

	  if ( yinfo.log() && rmin>0 && rmax>0 ) { 

	    /// calculate the log range
	    double delta = std::log10(rmax)-std::log10(rmin);

	    /// keep the original equation by way of documentation ...
	    //    ymaxset =  rmax*std::pow(10,delta*0.15*csize); 

	    yminset =  rmin*std::pow(10,-delta*0.1);

	    double newdelta = std::log10(rmax) - std::log10(yminset) + 0.05*delta;

	    if ( csize<10 ) ymaxset =  rmin*std::pow(10,newdelta/(1-0.07*csize));
	    else            ymaxset =  rmin*std::pow(10,newdelta*2);

	    if ( yminset!=yminset ) { 
	      std::cerr << " range error " << delta << " " << yminset << " " << ymaxset << "\t(" << rmin << " " << rmax << ")" << std::endl;
	      std::exit(-1);
	    }

	  }
	  else { 

	    /// calculate the required range such that the histogram labels 
	    /// won't crowd the points

	    if ( ypos>0.5 ) { 
	      double delta = rmax-rmin;
	    
	      yminset = rmin-0.1*delta;
	    
	      if ( rmin>=0 && yminset<=0 ) yminset = 0;
	    
	      double newdelta = rmax - yminset + 0.05*delta;
	    
	      if ( csize<10 ) ymaxset = yminset + newdelta/(1-0.09*csize);
	      else            ymaxset = yminset + newdelta*2;
	    }
	    else { 
	      double delta = rmax-rmin;
  
	      ymaxset = rmax+0.1*delta;
	    	    
	      double newdelta = ymaxset - rmin - 0.05*delta;
	    
	      if ( csize<10 ) yminset = ymaxset - newdelta/(1-0.09*csize);
	      else            yminset = ymaxset - newdelta*2;

	      if ( rmin>=0 && yminset<=0 ) yminset = 0;

	    }

	  }
	
	}
	else {  
	  if ( yinfo.rangeset() ) { 
	    yminset = yinfo.lo();
	    ymaxset = yinfo.hi();
	  }
	}
      
	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	// more useful debugging ...
	//      std::cout <<  "yauto: " << yinfo.autoset() << "\tyrange " << yinfo.rangeset() << std::endl;

	//      std::cout << "yminset " << yminset << "\tymaxset " << ymaxset << std::endl;  

	if ( yinfo.autoset() && yinfo.rangeset() ) {

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	  if ( yminset>yinfo.lo() ) yminset = yinfo.lo();
	  if ( ymaxset<yinfo.hi() ) ymaxset = yinfo.hi();
	}
      

	if ( contains(histo.name(),"_eff") ) {

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;
      
	  if ( effset ) { 
	    ymaxset = effmax;
	    yminset = effmin;
	  }
	}
      
	if ( ymaxset!=0 || yminset!=0 ) {

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	  plots.Max( ymaxset );
	  plots.Min( yminset );
	}

	if ( fulldbg ) std::cout << __LINE__ << std::endl;

	if ( yminset!=0 || ymaxset!=0 ) { 
	  if ( yminset>0 ) plots.SetLogy(yinfo.log());
	  else             plots.SetLogy(false);
	}	
	else plots.SetLogy(yinfo.log());

	//      plots.SetLogy(false);

	if ( fulldbg ) std::cout << __LINE__ << std::endl;
            
	//    plots.limits();
      
	/// actually draw the plot here ...
      
	if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;

	plots.Draw( legend );

	if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;

	if ( atlasstyle ) ATLASLabel( xpos, ypositions[0]+deltay, atlaslabel.c_str(), kBlack, ncolsp, nrowsp );

	if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;
          
	for ( unsigned it=0 ; it<taglabels.size() ; it++ ) { 
	  DrawLabel( xpos, ypositions[it], taglabels[it], kBlack, 0.04 );
	}
      }
    
      if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;


      if ( ( !nostats || !nomeans ) && !noplots ) { 
	if ( dochi2 ) for ( unsigned  j=0 ; j<Chi2.size() ; j++ ) DrawLabel( 0.75, 0.85-j*0.035, Chi2[j], colours[j%6] );
	if ( (contains(histo.name(),"_res") || 
	      contains(histo.name(),"1d")   ||
	      histo.name()=="pT"            || 
	      contains(histo.name(),"residual_") ||
	      contains(histo.name(),"vs_pt") ) && !contains(histo.name(),"sigma") ) { 

	  if ( contains(histo.name(),"_res") || contains(histo.name(),"residual_") || contains(histo.name(),"1d") ){
	    for ( unsigned j=0 ; j<chains.size() ; j++ ) { 
	      if ( !noreftmp ) { 
		if ( j<MeanRef.size() ) {
		  if ( !nomeans ) DrawLabel( xpos_original-0.02, (0.57-j*0.035), MeanRef[j], colours[j%6] );
		  DrawLabel( xpos_original-0.01, (0.57-0.035*chains.size()-j*0.035)-0.01, RMSRef[j],  colours[j%6] );
		}
	      }
	      if ( j<Mean.size() ) {
		if ( !nomeans ) DrawLabel( 0.62, (0.57-j*0.035), Mean[j],  colours[j%6] );
		DrawLabel( 0.62, (0.57-0.035*chains.size()-j*0.035)-0.01, RMS[j],  colours[j%6] );
	      }
	    }
	  }
	}
      
      }

      if ( xinfo.log() ) gPad->SetLogx(true);
      else               gPad->SetLogx(false);

      if ( yinfo.log() ) gPad->SetLogy(true);
      else               gPad->SetLogy(false);

      if ( fulldbg ) std::cout << __LINE__ << std::endl;

      if ( !noplots ) { 
      
	if ( make_ref_efficiencies ) { 
	
	  plots_eff.SetXaxisTitle( plots.GetXaxisTitle() ); 
	  plots_eff.SetYaxisTitle( plots.GetYaxisTitle() ); 
	
	  plots_eff.Draw( legend_eff );
	}
      
      } // no plots
    
    } /// loop over histograms in panel 


    /// NB: now, we no longer print the individual plots - only the "panels"
    ///     individual plots can still be printed by creating a "panel" which 
    ///     contains only that plots - in fast the "histos" item that before
    ///     provided details of the separate plots is now interpreted as 
    ///     a list of single plot panels so this is done for you

    if ( !noplots ) { 

      if ( !quiet ) { 

	tc->cd();

	std::string useplotname;

        if ( panel.size()>1 ) {
	  useplotname = panel.name();
	  replace( useplotname, "/", "_" );
	}	
	else { 
	  useplotname = plotname;
	}

	useplotname.erase( std::remove( useplotname.begin(), useplotname.end(), '+' ), useplotname.end() );

	// std::string printbase = dir+"HLT_"+ppanelname+tag;
	std::string printbase = dir + useplotname + tag;

	tc->Update();

	if ( !nopdf ) print_pad( printbase+".pdf" );
	if ( !nopng ) print_pad( printbase+".png" );
	if ( Cfile  ) print_pad( printbase+".C" );
	
	std::cout << std::endl;
      }
      
    }

    if ( tc ) delete tc;

  } /// loop over panels

  
  if ( fulldbg ) std::cout << __LINE__ << std::endl;
  
  /// if deleting all non-used reference histograms 
  
  
  /// make sure we are not using the same reference as test file
  bool files_duplicated = ( fref_==ftest_ );
  
  if ( deleteref && !files_duplicated ) {
    
    if ( fref_ ) { 
      
      /// clean up reference file
      
      std::cout << "main() cleaning up reference file" << std::endl;
      
      TFile* newout = new TFile(".newout.root","recreate"); 
      newout->cd();
      
      /// copy the release tree       
      copyReleaseInfo( fref, newout );
      
      TDirectory* base = gDirectory;
      
      for ( unsigned i=0 ; i<savedhistos.size() ; i++ ) { 
	
	/// debugging ...
	//	std::cout << i << " " << savedhistos[i] << std::endl;
	
	std::vector<std::string> dirs = AxisInfo::split( savedhistos[i], "/" );
	
	for ( unsigned j=0 ; j<dirs.size()-1 ; j++ ) { 
	  std::cout << "\t" << dirs[j] << std::endl;
	  TDirectory* renedir = gDirectory->GetDirectory( dirs[j].c_str() );
	  if ( renedir==0 ) gDirectory->mkdir( dirs[j].c_str() );
	  gDirectory->cd( dirs[j].c_str() );
	}
	

	TH1* href  = (TH1*)fref->Get( savedhistos[i].c_str() );
	if ( !noreftmp && href ) {
	  std::cout << i << " " << savedhistos[i] << " 0x" << href << std::endl;
	  href->Write( dirs.back().c_str() );
	}
      
          
	base->cd();
      }
    
      newout->Close();
    
    }
  }


  if ( fulldbg ) std::cout << __LINE__ << std::endl;
      
  /// close files

  if ( fref_ && !files_duplicated ) fref_->Close();
  if ( ftest_ ) ftest_->Close();
      
  /// now actually overwrite the old reference file

  if ( deleteref && !noref ) { 
    std::cout << "ref " << frefname << "\ttest " << ftestname << std::endl; 
    if ( frefname != ftestname && !files_duplicated ) { 
      std::string cmd = std::string("mv ") + frefname + " " + frefname + ".bak";
      std::system( cmd.c_str() );
      
      cmd = std::string("mv .newout.root ") + std::string(frefname);
      std::system( cmd.c_str() );
    }
    else {
      std::cerr << "reference file and test file are the same - not replacing" << std::endl;
    }
  }  
 
  //  std::cout << "deleting " << __LINE__ << std::endl;

  if ( fref_ && !files_duplicated ) delete fref_;
  if ( ftest_ ) delete ftest_;

  return 0;
}
