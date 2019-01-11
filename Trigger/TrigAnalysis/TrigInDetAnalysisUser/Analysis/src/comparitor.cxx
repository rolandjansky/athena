/**
 **     @file    comparitor.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 12 Oct 2012 13:39:05 BST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <cstdio>
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

/// get a histogram given a path, and an optional initial directory
/// if histogram is not found, then check for dir/name

template<typename T=TH1F>
T* Get( TFile& f, const std::string& n, const std::string& dir="", std::vector<std::string>* saved=0  ) { 

  std::string name;

  size_t pos = n.find("+");
  if ( pos!=std::string::npos ) name = n.substr( 0, pos );
  else                          name = n;

  T* h = (T*)f.Get( name.c_str() );
  if ( h || dir=="" || name.find(dir)!=std::string::npos ) { 
    std::cout << "Get() name " << name << " :: " << h << std::endl;
    if ( saved ) saved->push_back( name );
  }
  else { 
    h = (T*)f.Get( (dir+"/"+name).c_str() );
    std::cout << "Get() name " << (dir+"/"+name) << " :: " << h << std::endl;
    if ( saved ) { 
      if ( h ) saved->push_back( dir+"/"+name );
      else     saved->push_back( name );
    }
  }

  if ( h ) h->SetDirectory(0);

  return h;
}



int usage(const std::string& name, int status, const std::string& err_msg="" ) { 
  if ( err_msg != "" ) std::cerr << err_msg << "\n" << std::endl;
  std::ostream& s = std::cout;
  s << "Usage: " << name << "\t [OPTIONS]  test.root reference.root    chain1 chain2 chain2 ...\n\n"; 
  s << "\t" << " plots comparison histograms"; 
  s << " - compiled on " << __DATE__ << " at " << __TIME__ << "\n\n";
  //  s << "Configuration: \n";
  //  s << "    -o filename   \tname of output grid (filename required)\n\n";
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

  s << "         --unscalepix         \t do not scale the number of pixels by 0.5 (scaled by default)\n";
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
	//	h->SetBinContent( h->FindBin( i, j ), 1e-10 );
	//	h->SetBinError( h->FindBin( i, j ), 1e-10 );
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

  if ( argc<4 ) return usage(argv[0], -1, "too few arguments");

  /// control stuff
  
  std::string tag = "";
  std::string key = "";

  std::string dir = "";

  std::string ftestname = "";
  std::string frefname  = "";

  TFile* _ftest = 0;
  TFile* _fref  = 0;

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
  bool scalepix    = true;
  bool oldrms      = false;
  bool addchains   = false;
  bool usechainref = false;
  bool quiet       = false;

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
      taglabels.push_back( arg );
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
    else if ( arg=="-k" || arg=="--key" ) { 
      if ( ++i<argc ) key=argv[i];
      else return usage(argv[0], -1, "no key provided");
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
	std::string chain = arg;
	replace ( chain, ":", "_" );
	replace ( chain, ";", "_" );
	chains.push_back(chain);
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

  _ftest = TFile::Open( ftestname.c_str() );
  
  if ( noref==false ) { 
    if ( frefname=="" )  { 
      std::cerr << "main(): ref file not specified " << std::endl;
      return -1;
    }
    
    if ( frefname==ftestname )    _fref = _ftest;
    else if ( exists(frefname) )  _fref = TFile::Open( frefname.c_str() );
    else { 
      std::cerr << "main(): ref file " << frefname << " does not exist" << std::endl;
      return -1;
    }
  }
  else _fref = _ftest;
  
  if ( _ftest==0 || _fref==0 ) { 
    std::cerr << "could not open files " << std::endl;
    return -1;
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

  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadBottomMargin(0.15);

  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadTopMargin(0.05);

  std::cout << "Chains: " << std::endl;
  for ( unsigned ic=0 ; ic<chains.size() ; ic++ ) std::cout << "\t" << chains[ic] << std::endl;    

  if ( usrlabels.size()>0 ) std::cout << "labels " << usrlabels << std::endl;

  if ( usrlabels.size()>0 && usrlabels.size()==chains.size() ) uselabels = true;

  /// get release data

  TTree*   dataTree = 0;
  TString* releaseData = new TString("");
  std::vector<std::string> release_data;
  
  if ( !nowatermark ) { 
    dataTree = (TTree*)_ftest->Get("dataTree");
  
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
    std::cout << "trying to make directory" << std::endl;
    dir += "/";
    if ( !quiet && !noplots && !exists(dir) ) { 
      if ( mkdir( dir.c_str(), 0777 ) ) std::cerr << "main() couldn't create directory " << dir << std::endl;
      else                              std::cout << "main() output will be sent to directory " << dir << std::endl; 
    }
  }

  TFile& ftest = *_ftest;
  TFile& fref  = *_fref;

  std::string testrun = findrun( &ftest );

  std::string  rawrefrun = "";
  std::string  refrun = "";

  if ( _fref ) refrun = rawrefrun = findrun( &fref  );

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
  
  //  std::vector<std::string> chainnames;

  //  chainnames.resize(chains.size(),"");
  //  chainnames.clear();

  /// we divide the resolutions by these
  /// values if we have an "entries" histogram
  /// to tell us how many events there are
  /// overall
  int NeventTest = 1;
  int NeventRef  = 1;


  std::vector<std::string> savedhistos;

  
  if ( !nowatermark ) { 
    TH1D* htestev = (TH1D*)ftest.Get("event") ;
    TH1D* hrefev  = (TH1D*)fref.Get("event") ;

    std::cout << "htestev " << htestev << " " << hrefev  << std::endl;
    
    if ( htestev ) NeventTest = htestev->GetEntries(); 
    if ( hrefev )  NeventRef  = hrefev->GetEntries();
  
    savedhistos.push_back("event");
  }
  else { 
    NeventTest = 1; /// get from Chains? 
    NeventRef  = 1; 
  }


  /// set up the correct reference chain names ...

  std::cout << "\ncreating chain and reference information ..." << std::endl;

  for ( size_t j=0; j<chains.size(); j++)  {
	
    /// get the actual chain name and track collection from 

    /// the Chain histogram if present
    std::cout << "chain:        " << chains[j] << "\taddchains: " << addchains << std::endl;
    //    std::cout << "chainref:     " << chainref[j] << " - original" << std::endl;
    
    if ( addchains && ( contains(chains[j],"Shifter") || ( !contains(chains[j],"HLT_") && !contains(chains[j], "Fullscan" ) ) ) ) { 
      
      TH1F* hchain = Get( ftest, chains[j]+"/Chain", testrun );
      
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


  for ( size_t j=0 ; j<chainref.size() ; j++ ) { 
    std::cout << "chainref: " << chainref[j] << " :: " << refchains[j] << std::endl;
    if ( chainref[j]!="" ) refchain[j] = fullreplace( chainref[j], pattern, regex );
    else                   refchain[j] = fullreplace( refchains[j], pattern, regex );
    std::cout << "refchain: " << refchain[j] << std::endl;
  }


  /// set up the plot panels 

  std::vector<Panel> panels;

  /// read config in from a file if requested ...

  if ( configfile!="" ) { 

    if ( exists(configfile) ) { 

      std::cout << argv[0] << ":\treading histogram configuration from file " << configfile << std::endl; 
    
      ReadCards rc(configfile);


      /// read the histos - 1 panel per histo
      
      if ( rc.isTagDefined( "histos" ) ) { 
	
	std::vector<std::string> raw_input = rc.GetStringVector( "histos" );
	
	for ( size_t iraw=0 ; iraw<raw_input.size() ; iraw += 6) {
	  HistDetails h( &(raw_input[iraw]) );	
	  Panel p( h.name(), 1 ); 
	  p.push_back( h );
	  panels.push_back( p );
	}
	
      }
      

      /// read in the panel descriptions

      if ( rc.isTagDefined( "panels" ) ) { 
	
	std::vector<std::string> panel_config = rc.GetStringVector( "panels" );
	
	for ( size_t ipanel=panel_config.size() ; ipanel-- ;  ) { 
	  
	  std::vector<std::string> raw_input = rc.GetStringVector( panel_config[ipanel] );
	  
	  Panel p( panel_config[ipanel], ncols );
	  
	  if ( raw_input.empty() ) throw std::exception();
	  for ( size_t iraw=0 ; iraw<raw_input.size() ; iraw += 6 ) p.push_back( HistDetails( &(raw_input[iraw]) ) );
	  	  
	  panels.push_back( p ); 

	}

      }	  
      
    }
    else { 
      std::cerr << argv[0] << ":\t config file not found: " << configfile << std::endl;
      return -1;
    }

  }
  else { 


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

    std::string ((*inpanels[3])[6]) = { eff_panel, res_panel, diff_panel };

    size_t nphist[3] = { 4, 4, 10 }; 

    std::string pnames[3] = { "eff", "res", "diff" };

    for ( size_t ip=0 ; ip<3 ; ip++ ) { 
      Panel p( pnames[ip]+"_panel", 2 );
      for ( size_t iraw=0 ; iraw<nphist[ip] ; iraw++ ) p.push_back( HistDetails( inpanels[ip][iraw] ) );
      panels.push_back( p );
    }

  }

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
      
      Plots plots_eff;
      plots_eff.clear();
      
      Plots plots;
      plots.clear();
      
      
      //      TCanvas* c1 = new TCanvas(label("canvas-%d",i).c_str(),"histogram",800,600);
      //      c1->cd();
      
      
      double xpos  = 0.18;
      double ypos  = 0.93;
      
      if ( contains(histo.name(),"eff") || contains(histo.name(),"Eff_") ) ypos = 0.15;
      
      if ( atlasstyle ) { 
	xpos  = 0.18;
	if ( ypos>0.5 ) ypos = 0.85; 
	else            ypos = 0.18;
      }
      
      if ( _ypos!=0 ) ypos = _ypos;
      
      double xpos_original = xpos;
      
      xpos += xoffset;
      
      /// calculate all the postions for the items in the legend
      
      int Nlines = chains.size() + taglabels.size();
      
      std::vector<double> ypositions;
      
      double deltay = (chains.size()*0.06-0.005)/chains.size();
      
      double ylo = ypos;
      double yhi = ypos;
      
      if ( ypos>0.5 ) ylo -= Nlines*deltay;
      else            yhi += Nlines*deltay;
      
      for ( int ilines=0 ; ilines<Nlines ; ilines++ ) { 
	ypositions.push_back( yhi - deltay*(ilines+0.5) );
      }

      //      bool residual = false;

      //      if ( contains(histo.name(),"_res") ||  contains(histo.name(),"residual_") || contains(histo.name(),"1d") ) residual = true; 
      
      //      if ( residual ) xpos = xpos_original;
      
      // specify different legends for efficiencies or residuals?
      
      /// legends ....
      Legend legend( xpos, xpos+0.1, ylo, ylo+chains.size()*0.06-0.005 );
      Legend legend_eff( xpos, xpos+0.1, ylo, ylo+chains.size()*0.06-0.005 );
            
      
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
      
      
      
      //    bool uselogx = xinfo.log();
      //    bool uselogy = yinfo.log();
      
      for ( unsigned int j=0; j<chains.size(); j++)  {
	
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
	
	
	std::cout << "refchain: " << refchain[j] << std::endl;
	
	/// refit the resplots - get the 2d histogram and refit
	
	if ( refit_resplots && ( contains(histo.name(),"/sigma") || contains(histo.name(),"/mean") ) ) { 
	  
	  bool bsigma = false;
	  if ( contains(histo.name(),"/sigma") ) bsigma = true;

	  bool bmean = false;
	  if ( contains(histo.name(),"/mean") ) bmean = true;

	  //    	    std::cout << "\trefitting:  " << histos[i] << std::endl;
	    
	  Resplot::setoldrms95(oldrms);
	  Resplot::setscalerms95(true);
	    
	  std::string tmp_  = histo.name();
	  std::string base;
	    
	  if ( bsigma ) base = chop( tmp_, "/sigma" );
	  if ( bmean )  base = chop( tmp_, "/mean" );
	    
	  //	  TH2D* _htest2d = (TH2D*)ftest.Get((chains[j]+"/"+base+"/2d").c_str()) ;
	  //	  TH2D* _href2d  = (TH2D*)ftest.Get((refchain[j]+"/"+base+"/2d").c_str()) ;
	    
	  //	  savedhistos.push_back( chains[j]+"/"+base+"/2d" );	    

	  TH2D* _htest2d = Get<TH2D>( ftest, chains[j]+"/"+base+"/2d", testrun, &savedhistos );
	  TH2D* _href2d  = Get<TH2D>( ftest, refchain[j]+"/"+base+"/2d", testrun  );

	  if ( _htest2d==0 ) continue;
	  if ( !noreftmp && _href2d==0 )  noreftmp = true;
	  Plotter::setplotref(!noreftmp);
	    
	  /// get the test histogram
	    
	  //	    std::cout << "test " << _htest2d << std::endl;
	  Resplot rtest("tmp", _htest2d);
	  //	    if ( contains(histos[i],"npix") || contains(histos[i],"nsct") ) rtest.Finalise(Resplot::FitNull);
	  //      else   rtest.Finalise(Resplot::FitNull95);
	  if ( rtest.finalised() ) { 
	    if ( contains(histo.name(),"npix") || contains(histo.name(),"nsct") || contains(histo.name(),"nsi") || contains(histo.name(),"nbl") ) rtest.Refit(Resplot::FitNull);
	    else  rtest.Refit(Resplot::FitNull95);
	  }
	  else {
	    if ( contains(histo.name(),"npix") || contains(histo.name(),"nsct") || contains(histo.name(),"nsi") || contains(histo.name(),"nbl") ) rtest.Finalise(Resplot::FitNull);
	    else  rtest.Finalise(Resplot::FitNull95);
	  }

	  if ( bsigma ) { htest = (TH1F*)rtest.Sigma()->Clone("rtest_sigma"); htest->SetDirectory(0); }
	  if ( bmean  ) { htest =  (TH1F*)rtest.Mean()->Clone("rtest_mean");  htest->SetDirectory(0); }

	  if ( htest==0 ) { 
	    std::cerr << "missing histogram: " << (refchain[j]+" / "+histo.name()) << " " << htest << "(test)" << std::endl; 
	    continue;
	  }


	  /// Actually D) refit the references - if we are adding together many
	  /// resplots, then the means etc may not even have been calculated
	  /// 
	  /// NB: DON'T Refit the reference, since only the central values
	  ///     are plotted
	  //	 std::cout << "ref " << _href2d << std::endl;
	  //   Resplot rref("tmp", _href2d);
	  //   rref.Finalise(Resplot::FitNull95);
	  //   href = (TH1F*)rref.Sigma()->Clone("rref_sigma"); href->SetDirectory(0);

	  /// still get the reference histo

	  //	    href  = (TH1F*)fref.Get((chains[j]+"/"+histos[i]).c_str()) ;

	  //	    TH1F* hreft  = (TH1F*)fref.Get( (refchain[j]+"/"+histos[i]).c_str() );
	
	  TH1F* hreft  = 0;

	  if ( !noreftmp ) {
	    if ( refitref_resplots ) { 
	      //	    std::cout << "test " << _htest2d << std::endl;
	      Resplot rref("tmp", _href2d);
		
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
	      hreft = Get( fref, refchain[j]+"/"+histo.name(), rawrefrun );
	    }
	  }
	      
	  if ( !noreftmp && hreft==0 ) { 
	    std::cerr << "missing histogram: " << (refchain[j]+" / "+histo.name()) << " " << htest << "(ref)" << std::endl; 
	    noreftmp = true; 
	    Plotter::setplotref(!noreftmp);
	  }
	      
	  if ( !noreftmp ) { 
	    href = (TH1F*)hreft->Clone();
	    href->SetDirectory(0);
	  }
	    	      
	  //	    std::cout << "\tget " << (refchain[j]+"/"+histos[i]) << "\t" << href << std::endl;
	    
	  savedhistos.push_back( refchain[j]+"/"+histo.name() );
   	   
	}
	else { 

	  if ( fulldbg ) std::cout << __LINE__ << std::endl;

	  /// everything else 
       	
	  std::string reghist = histo.name();

	  std::cout << "hist:  " << (chains[j]+"/"+reghist) << std::endl;

	  htest = Get( ftest, chains[j]+"/"+reghist, testrun, &savedhistos );

	  //	  htest = (TH1F*)ftest.Get((chains[j]+"/"+reghist).c_str(), &savedhistos ) ;
	  //	  savedhistos.push_back( chains[j]+"/"+reghist );

	  //	  if ( htest==0 ) { 
	  //	    htest = (TH1F*)ftest.Get((testrun+"/"+chains[j]+"/"+reghist).c_str()) ;
	  //	    savedhistos.push_back( testrun+"/"+chains[j]+"/"+reghist );
	  //	  }

	  if ( htest==0 ) { 
	    if ( htest==0 ) std::cerr << "missing histogram: " << (chains[j]+" / "+reghist) << " " << htest<< std::endl; 
	    continue;
	  }


	  TH1F* hreft = Get( fref, refchain[j]+"/"+reghist, rawrefrun );

	  //	  TH1F* hreft  = Get( fref, refchain+"/"+reghist, rawrefrun );
	  //	  TH1F* hreft  = (TH1F*)fref.Get((refchain+"/"+reghist).c_str()) ;
	
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
	    if ( hreft==0 ) std::cerr << "missing histogram: " << (refchain[j]+" / "+reghist)  << " " << hreft << std::endl; 
	    noreftmp = true;
	    Plotter::setplotref(false);
	  }
	
	  if ( fulldbg ) std::cout << "htest: " << htest << std::endl; 
	  if ( fulldbg ) std::cout << "hreft: " << hreft << std::endl; 

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

	    //	  std::cout << "contains _eff " << contains( std::string(htest->GetName()), "eff" ) << std::endl;

	    if ( contains( std::string(htest->GetName()), "_eff" ) ) {

	      htestnum = Get( ftest, chains[j]+"/"+histo.name()+"_n", testrun );
	      //	      htestnum = (TH1F*)ftest.Get((chains[j]+"/"+histo.name()+"_n").c_str()) ;

	      TH1F* hrefnumt  = Get( fref, refchain[j]+"/"+histo.name()+"_n", rawrefrun, &savedhistos );

	      //      TH1F* hrefnumt  = (TH1F*)fref.Get((refchain+"/"+histo.name()+"_n").c_str()) ;
	      //      savedhistos.push_back( refchain+"/"+histo.name()+"_n" );
	   

	      if ( !noreftmp && hrefnumt!=0 ) { 
		hrefnum = (TH1F*)hrefnumt->Clone();
		hrefnum->SetDirectory(0);
	      }

	    
	      //	    std::cout << "numerator histos " << htestnum << " " << hrefnum << std::endl;

	    }
	  }
	}


	if ( _bayes ) { 

	  if ( htest && contains( std::string(htest->GetName()), "eff" ) ) {

	    //	  delete htest;

	    //	    htestnum = (TH1F*)ftest.Get((chains[j]+"/"+histo.name()+"_n").c_str()) ;
	    //	    htestden = (TH1F*)ftest.Get((chains[j]+"/"+histo.name()+"_d").c_str()) ;

	    //	    savedhistos.push_back( chains[j]+"/"+histo.name()+"_n" );
	    //	    savedhistos.push_back( chains[j]+"/"+histo.name()+"_d" );


	    htestnum = Get( ftest, chains[j]+"/"+histo.name()+"_n", testrun, &savedhistos ) ;
	    htestden = Get( ftest, chains[j]+"/"+histo.name()+"_d", testrun, &savedhistos ) ;

	    std::cout << "Bayesian error calculation " << htestnum << " " << htestden << "\tscale " << scale_eff << std::endl;

	    if ( htestnum && htestden ) { 

#if 0
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
    
	      Efficiency e( htestnum, htestden, "", scale_eff );
	      tgtest = e.Bayes(scale_eff);

	      htest = e.Hist();

	    }
	
	    /// now recalculate reference

	    std::cout << "recalculating reference efficiencies ..." << std::endl; 

	    if ( href ) { 

	      //	  delete htest;

	      std::cout << "doin ..." << std::endl; 

	      //	      TH1F* hrefnum = (TH1F*)fref.Get((refchain+"/"+histo.name()+"_n").c_str()) ;
	      //	      TH1F* hrefden = (TH1F*)fref.Get((refchain+"/"+histo.name()+"_d").c_str()) ;
	      TH1F* hrefnum = Get( fref, refchain[j]+"/"+histo.name()+"_n", rawrefrun );
	      TH1F* hrefden = Get( fref, refchain[j]+"/"+histo.name()+"_d", rawrefrun );

	      std::cout << "Bayesian error calculation " << htestnum << " " << htestden << "\tscale " << scale_eff     << std::endl;
	      std::cout << "Bayesian error calculation " << hrefnum  << " " << hrefden  << "\tscale " << scale_eff_ref << std::endl;
	    
	      if ( hrefnum && hrefden ) { 
		Efficiency e( hrefnum, hrefden, "", scale_eff_ref );
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


	htest->GetYaxis()->SetTitleOffset(1.5); 
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
	  htest->Rebin(2);
	  htest->Sumw2();
	  if ( !noreftmp ) { 
	    href->Rebin(2);
	    href->Sumw2();
	  }
	  //    htest->Scale(1./NeventTest);
	  //    href->Scale(1./NeventRef);
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



	if ( actual_chain.find("_idperf")!=std::string::npos )    actual_chain.erase( actual_chain.find("_idperf"), 7 );
	if ( actual_chain.find("_bperf")!=std::string::npos )     actual_chain.erase( actual_chain.find("_bperf"), 6 );
	if ( actual_chain.find("_boffperf")!=std::string::npos )  actual_chain.erase( actual_chain.find("_boffperf"), 9 );
	if ( collection.find("_IDTrkNoCut")!=std::string::npos )  collection.erase( collection.find("_IDTrkNoCut"), 11 );
	if ( collection.find("xAODCnv")!=std::string::npos )      collection.erase( collection.find("xAODCnv"), 7 );
	if ( collection.find("Tracking")!=std::string::npos )     collection.replace( collection.find("Tracking"), 8, "Trk" );    
	if ( collection.find("InDetTrigTrk_")!=std::string::npos ) collection.erase( collection.find("InDetTrigTrk_"), 13 );    
	if ( collection.find("FTK_Track")==std::string::npos ) replace( collection, "_Tr", " :  " );

	std::string c = actual_chain + " : " + collection;

	std::cout << "track collection: " << collection   << "   <-" << std::endl;
	std::cout << "actual chain:     " << actual_chain << "   <-" << std::endl;
       
	replace( c, "_In", " :  " );

	c = "  " + c;

	std::cout << "use label: " << c << std::endl;

	/// calculate and set axis limits

	//      std::cout << "adding plot " << histos[i] << " " << htest->GetName() << std::endl;

	if ( fulldbg ) std::cout << __LINE__ << std::endl;
      
	

	//      if ( uselabels )  plots.push_back( Plotter( htest, href, chain_name+usrlabels[j], tgtest ) );
	if ( uselabels )  plots.push_back( Plotter( htest, href, " " + chain_name[j] + usrlabels[j], tgtest ) );
	else {
	  std::cout << "using label: " << c << std::endl;
	  plots.push_back( Plotter( htest, href, c, tgtest ) );
	}

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

	if( residual ) {
	
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
      
	///    if ( contains(histo.name(),"_res"))  plots.xrange(true);
	//      if ( contains(histo.name(),"_res") ) plots.MaxScale( 100 ); 
      
	//    plots.limits();
      
	/// actually draw the plot here ...
      
	if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;

	plots.Draw( legend );

	if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;

	//      if ( atlasstyle ) ATLASLabel( xpos, ypositions[0]+deltay, atlaslabel.c_str() );

	if ( atlasstyle ) ATLASLabel( xpos, ypositions[0]+deltay, atlaslabel.c_str(), kBlack, ncolsp, nrowsp );

	if ( fulldbg ) if ( fulldbg ) std::cout << __LINE__ << std::endl;
          
	for ( unsigned it=0 ; it<taglabels.size() ; it++ ) { 
	  //      std::cout << "\ttaglabel " << ypositions[it] << "\t(" << histo.name() << ")" << std::endl;
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
		//	      std::cout << "\tdraw stats " << histo.name() << "\tMean " << Mean[j] << std::endl;
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
  bool files_duplicated = ( _fref==_ftest );
  
  if ( deleteref && !files_duplicated ) {
    
    if ( _fref ) { 
      
      /// clean up reference file
      
      std::cout << "main() cleaning up reference file" << std::endl;
      
      //      TFile* newout = new TFile(".newout.root","recreate"); 
      TFile* newout = new TFile(".newout.root","recreate"); 
      newout->cd();
      
      /// copy the release tree       
      copyReleaseInfo( &fref, newout );
      
      TDirectory* base = gDirectory;
      
      for ( unsigned i=0 ; i<savedhistos.size() ; i++ ) { 
	
	//	std::cout << i << " " << savedhistos[i] << std::endl;
	
	std::vector<std::string> dirs = AxisInfo::split( savedhistos[i], "/" );
	
	for ( unsigned j=0 ; j<dirs.size()-1 ; j++ ) { 
	  std::cout << "\t" << dirs[j] << std::endl;
	  TDirectory* renedir = gDirectory->GetDirectory( dirs[j].c_str() );
	  if ( renedir==0 ) gDirectory->mkdir( dirs[j].c_str() );
	  gDirectory->cd( dirs[j].c_str() );
	}
	
	TH1* href  = (TH1*)fref.Get( savedhistos[i].c_str() );
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

  if ( _fref && !files_duplicated ) _fref->Close();
  if ( _ftest ) _ftest->Close();
      
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

  if ( _fref && !files_duplicated ) delete _fref;
  if ( _ftest ) delete _ftest;

  return 0;
}
