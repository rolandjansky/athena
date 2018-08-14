//
//   @file    cpucost.cxx
//
//
//   @author B.Sowden
//
//   Copyright (C) 2014 B.Sowden (benjamin.sowden@cern.ch)
//
//   $Id: cpucost.cxx, v0.0   Mon 04 Aug 2014 10:45:00 BST bsowden $


#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h> 

#include <iostream>
#include <string>
#include <vector>

#include "utils.h"
#include "label.h"
#include "DrawLabel.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TSystem.h"

#include "TKey.h"
#include "TList.h"
#include "TDirectory.h"

#include "computils.h"

/// Prints usage instructions to standard output and returns given status
int usage(const std::string& name, int status) {
  std::ostream& s = std::cout;
  s << "Usage: " << name << " [OPTIONS] expert-monitoring.root reference.root algorithm1 algorithm2 algorithm3 ...\n\n";
  s << "  TIDA \'" << name << "\' extracts timing histograms\n\n";
  s << "Options: \n";
  s << "    -o,  --outputfolder value\t puts output in folder 'value' making it if it doesn't exist, \n";
  s << "    -t,  --tag value         \t appends tag 'value' to the end of output plot names, \n";
  s << "    -k,  --key value         \t prepends key 'value' to the front of output plot names, \n";
  s << "    -a,  --auto              \t process all histograms that are in the file, \n";
  s << "    -d,  --directory value   \t if auto is set, search only in specifed directory, \n";
  s << "    -p,  --pattern value     \t if auto is set, search for histograms containing this string, \n";
  s << "    -nr, --noref             \t do not use a reference file, \n";
  s << "    -v,  --verbose           \t verbose output\n";
  s << "    -h,  --help              \t this help\n";
  s << std::endl;
  return status;
}



struct histoinfo {
  histoinfo( const std::string& f, const std::string& d ) : fname(f), dname(d) { } 
  std::string fname; // File name
  std::string dname; // Display name
};


std::ostream& operator<<( std::ostream& s, const histoinfo& h ) { 
  return s << h.fname << " : " << h.dname; 
}



int main(int argc, char** argv) {

  if (argc < 4) { return usage(argv[0], -1); }

  std::string dir = "";
  std::string tag = "";
  std::string key = "";

  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.05);

  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadBottomMargin(0.14);
  //  gStyle->SetTitleXOffset(0.1);
  //  gStyle->SetTitleYOffset(0.1);


  TFile* ftest = 0;
  TFile* fref  = 0;

  bool atlasstyle = false;
  bool ylog = true;
  bool nopng = false;

  bool autochains = false;

  std::string autopattern = "";

  std::vector<std::string> taglabels;

  std::string directory = "TIMERS";
  std::string pattern = "_TotalTime";

  TDirectory* tdir = gDirectory;

  bool verbose = false;
  
  bool noref = false;

  std::string frefname = "";

  // Parse the arguments
  std::vector<std::string> algorithms;
  for(int argnum = 1; argnum < argc; argnum++){
    std::string arg = argv[argnum];

    if (arg == "-h" || arg == "--help") {
      return usage(argv[0], 0);
    }
    else if (arg == "-o" || arg == "--outputfolder") {
      if (++argnum < argc) { dir = argv[argnum]; } 
      else { return usage(argv[0], -1); }
    }
    else if (arg == "-t" || arg == "--tag") {
      if (++argnum < argc) { tag = std::string("-") + argv[argnum]; }
      else { return usage(argv[0], -1); }
    }
    else if (arg == "-k" || arg == "--key") {
      if (++argnum < argc) { key = argv[argnum] + std::string("-"); }
      else { return usage(argv[0], -1); }
    }
    else if (arg == "-np" || arg == "--nopng") {
      nopng = true;
    }
    else if (arg == "-a" || arg == "--auto") {
      autochains = true;
    }
    else if (arg == "-v" || arg == "--verbose") {
      verbose = true;
    }
    else if (arg == "-nr" || arg == "--noref") {
      noref = true;
    }
    else if (arg == "-ap" || arg == "--autopattern") {
      if (++argnum < argc) autopattern = argv[argnum];
      else                 return usage(argv[0], -1); 
    }
    else if (arg == "-d" || arg == "--directory") {
      if (++argnum < argc) directory = argv[argnum];
      else                 return usage(argv[0], -1); 
    }
    else if (arg == "-p" || arg == "--pattern") {
      if (++argnum < argc) pattern = argv[argnum];
      else                 return usage(argv[0], -1); 
    }
    else {
      if (ftest == 0) {
	std::string file = globbed(arg);
        if (exists(file)) {
          ftest = new TFile( file.c_str() );
        }
        else {
          std::cerr << "main(): test file " << arg << " does not exist" << std::endl;
          return -2;
        }
      }
      else if ( frefname=="" ) frefname = arg;
      else {
        algorithms.push_back(arg);
      }
    }
  }
  
  if (ftest == 0 || ( noref==false && frefname=="" ) ) {
    return usage(argv[0], -4);
  }
  
  if ( fref == 0 && !noref ) {
    std::string file = globbed(frefname);
    if (exists(file)) {
      fref = new TFile( file.c_str() );
    }
    else {
      std::cerr << "main(): ref file " << frefname << " does not exist" << std::endl;
      return -3;
    }
  }

  if ( noref ) fref = ftest;

  if ( noref ) Plotter::setmeanplotref(!noref);

  if ( ftest && autochains ) { 

    ftest->cd();

    std::vector<std::string> dirs;
    contents( dirs, gDirectory, directory, pattern );


    if ( autopattern=="" ) { 
      for ( unsigned j=0 ; j<dirs.size() ; j++ ) { 
	if ( verbose ) std::cout << "\talgorithm " << dirs[j] << std::endl;
	algorithms.push_back( dirs[j] );
      }
    }
    else { 
      std::cout << "autopattern : " << autopattern << std::endl; 
      for ( unsigned j=0 ; j<dirs.size() ; j++ ) { 
	if ( dirs[j].find(autopattern)!=std::string::npos ) { 
	  algorithms.push_back( dirs[j] );
	  std::cout << "adding " << algorithms.back() << std::endl;
	}
      }
    }

  }
  


  // Make output directory
  if ( dir != "" ) {
    if ( mkdir( dir.c_str(), 0777 ) ) { 
      if ( exists(dir) ) std::cerr << "main() directory " << dir << " aleady exists" << std::endl; 
      else               std::cerr << "main() could not create directory " << dir << std::endl; 
    }
    dir += "/";
  }

#if 0
  // Get the timers directories from input files
  //  TDirectoryFile* testtimers = 0;
  TDirectory* testtimers = 0;

  if ( directory!="" ) ftest->GetObject( directory.c_str(), testtimers );
  else                 testtimers = ftest;

  if (testtimers == 0 ) {
    std::cerr << "main(): can not find timers in test file" << std::endl;
    //    return -1;
  }

  //  TDirectoryFile* reftimers = 0;
  TDirectory* reftimers = 0;

  if ( directory!="" ) fref->GetObject( directory.c_str(), reftimers );
  else                 reftimers = fref;

  if (reftimers == 0 ) {
    std::cerr << "main(): can not find timers in ref file" << std::endl;
    // return -1;
  }
#endif

  TFile fcck( "fcck.root", "recreate" );


  std::vector<histoinfo> histograms;
  histograms.push_back( histoinfo("_TotalTime", "Total time") );

  // Provide output to user for progress status
  //  std::cout << "main() processing algorithms : " << algorithms << std::endl;

  // Loop over histograms
  //  for (unsigned int histogram = 0; histogram < histograms.size(); ++histogram) {
  for (unsigned int histogram=histograms.size(); histogram-- ; ) {

    std::cout << "\nhistogram " << histograms.at(histogram) << " : with " << algorithms.size() << " algorithms" << std::endl;


    std::string xaxis = histograms.at(histogram).dname + " [ms]";
    std::string yaxis = "Entries";

    
    // Loop over input algorithms
    //    for (unsigned int algorithm = 0; algorithm < algorithms.size(); ++algorithm) {
    for (unsigned int algorithm = algorithms.size(); algorithm-- ; ) {

      std::cout << "\nmain() processing algorithm : " << algorithms[algorithm] << std::endl;

      TCanvas* c1 = new TCanvas( label("canvas-%d",int(histogram)).c_str(), "histogram", 800, 600 );
      c1->cd();
            
      double x1 = 0.17;
      double x2 = 0.25;
      double y1 = 0.69;
      double y2 = 0.90;

      /// adjust the legend if no reference times are to be plotted
      if ( noref ) y1 = y2-0.5*(y2-y1);

      Legend legend(x1, x2, y1, y2);
     

      std::string histname = algorithms[algorithm]; // + histograms.at(histogram).fname;

      std::string _xaxis = xaxis;
      bool fractional = contains( histname, "Fractional" );
      if ( fractional ) _xaxis = "Fraction of " + histograms.at(histogram).dname;
      

      //      std::cout << "\t" << histname << "\t" << algorithms.at(algorithm) << " " <<  histograms.at(histogram).fname << std::endl;


      //      std::cout << "Directory: " << gDirectory->GetName() << std::endl;

      TH1F* testhist = (TH1F*)ftest->Get(histname.c_str());
      if (testhist == 0 ) {
        std::cerr << "main(): can not find hist " << histname << " in test file" << std::endl;
        continue;
      }
      
      
      testhist->SetName( tail(algorithms[algorithm],"/").c_str() );
      testhist->Write(); 

      //      std::cout << "\n\nfound histname " << histname << std::endl;

      TH1F* refhist = (TH1F*)fref->Get(histname.c_str());
  
      if (refhist == 0 ) {
        std::cerr << "main(): can not find hist " << histname << " in ref file" << std::endl;
        continue;
      }


      testhist->GetYaxis()->SetTitleOffset(1.5);
      refhist->GetYaxis()->SetTitleOffset(1.5);
      testhist->GetXaxis()->SetTitle(_xaxis.c_str());
      testhist->GetYaxis()->SetTitle(yaxis.c_str());
      refhist->GetXaxis()->SetTitle(_xaxis.c_str());
      refhist->GetYaxis()->SetTitle(yaxis.c_str());

      Plots plots;

      std::string algname = tail(algorithms[algorithm], "/" );
      if ( algname.find("h_")==0 ) algname.erase(0, 2);
    
      //      size_t indetpos = algname.find("InDet");
      //      if ( indetpos!=std::string::npos ) algname.erase( indetpos, 5);

      plots.push_back( Plotter( testhist, refhist, " "+algname ) );

      std::string plotname = dir + key + algname + tag;
      //                              histograms.at(histogram).fname + tag;


      std::cout << "dir " << dir << "\tkey " << key << "\talgname " << algname << "\ttag " << tag << std::endl;  

      //      std::cout << "testhist " << testhist << " " << refhist << std::endl;


      std::vector<std::string> chains;
      chains.push_back( algname + tag );

      bool _ylog = ylog;
      
      double Nent     = plotable( testhist );
      double Nent_ref = plotable( refhist ); 
     
      if ( fractional ) _ylog = false;

      if ( Nent==0 || Nent_ref==0 ) { 
	_ylog = false;
	std::cerr << "histograms empty: " << testhist->GetName() << std::endl;
	continue;
      }


      //      if ( ylog || histograms.at(histogram).fname == "_TotalTime") {
        testhist->SetTitle("");
        refhist->SetTitle("");
	//    c1->SetLogy(true);
	// plots.xrange();
	//        plots.Max(5);

        plots.SetLogy(_ylog);


	double rmin = plots.realmin();
	double rmax = plots.realmax();
	
	if ( _ylog ) { 
	  double delta = std::log10(rmax)-std::log10(rmin);
	  if ( atlasstyle ) plots.Max( rmax*std::pow(10,delta*0.15*2*(chains.size()+taglabels.size()+1)) );
	  else              plots.Max( rmax*std::pow(10,delta*0.15*2*(chains.size()+taglabels.size())) );
	  plots.Min( rmin*std::pow(10,-delta*0.1) );
	}
	else { 
	  double delta = rmax-rmin;
	  plots.Max( rmax+delta*0.1*2*chains.size() );

	  double pmin = rmin-delta*0.1; 
	  if ( pmin>0 ) plots.Min( pmin );
	  else          plots.Min( 0 );
	  
	}
	
	//      }
	//      else {
	//	plots.SetLogy(false);
	//        c1->SetLogy(false);
	//   }

      plots.Draw( legend, true );

      plots.back().Print( (plotname+".pdf").c_str() );
      if ( !nopng ) plots.back().Print( (plotname+".png").c_str() );

      delete c1;

      std::cout << "done algorithm " << algorithm << " " << algorithms[algorithm] << std::endl;
    }

    std::cout << "done hist " << histogram << " " << histograms.at(histogram).dname << " " << std::endl;
  }

  fcck.Write();
  fcck.Close();

  tdir->cd();


#ifdef USE_SLOW_ROOT_FILE_DELETION

  std::cout << "deleting ftest" << std::endl;

  /// AAAAARGH!!!! these deletes do not work!!! they just sit there for ever!!
  ///              what is wrong with these destructors !!! I ask you


  if ( fref!=ftest ) { 
    std::cout << "deleting fref" << std::endl;  
    
    //  delete reftimers;
    delete fref;
  }

  //  delete testtimers;
  delete ftest;

#endif

  std::cout << "done" << std::endl;

  return 0;
}
