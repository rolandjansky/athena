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
  s << "    -o, --outputfolder value\t puts output in folder 'value' making it if it doesn't exist, \n";
  s << "    -t, --tag value\t appends tag 'value' to the end of output plot names, \n";
  s << "    -k, --key value\t prepends key 'value' to the front of output plot names, \n";
  s << "    -a, --auto     \t plot all algorithms that are i the file, \n";
  s << "    -h, --help     \t this help\n";
  s << std::endl;
  return status;
}



int main(int argc, char** argv) {

  if (argc < 4) { return usage(argv[0], -1); }

  std::string dir = "";
  std::string tag = "";
  std::string key = "";

  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.05);

  TFile* ftest = 0;
  TFile* fref  = 0;

  bool autochains = false;

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
    else if (arg == "-a" || arg == "--auto") {
      autochains = true;
    }
    else {
      if (ftest == 0) {
        if (exists(arg)) {
          ftest = new TFile(arg.c_str());
        }
        else {
          std::cerr << "main(): test file " << arg << " does not exist" << std::endl;
          return -1;
        }
      }
      else if (fref == 0) {
        if (exists(arg)) {
          fref = new TFile(arg.c_str());
        }
        else {
          std::cerr << "main(): ref file " << arg << " does not exist" << std::endl;
          return -1;
        }
      }
      else {
        algorithms.push_back(arg);
      }
    }
  }

  if (ftest == 0 || fref == 0 ) {
    return usage(argv[0], -1);
  }


  if ( ftest && autochains ) { 

    ftest->cd();

    TList* tl  = gDirectory->GetListOfKeys();

    for ( int i=tl->GetSize() ; i-- ; ) {

      TKey* tobj = (TKey*)tl->At(i);

      if ( tobj==0 ) continue;
      
      if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
	
	TDirectory* tnd = (TDirectory*)tobj->ReadObj();
	
	if ( tnd==0 ) continue;

	std::string dname = tnd->GetName();

	std::cout << "main(): will process \t" << dname << std::endl; 
	algorithms.push_back( dname );
		
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

  // Get the timers directories from input files
  TDirectoryFile* testtimers = 0;
  ftest->GetObject("TIMERS",testtimers);
  if (testtimers == 0 ) {
    std::cerr << "main(): can not find timers in test file" << std::endl;
    return -1;
  }

  TDirectoryFile* reftimers = 0 ;
  fref->GetObject("TIMERS",reftimers);
  if (reftimers == 0 ) {
    std::cerr << "main(): can not find timers in ref file" << std::endl;
    return -1;
  }



  // Define the histograms we are interested in
  struct histogramsandnames {
    std::string fname; // File name
    std::string dname; // Display name
  };

  std::vector<histogramsandnames> histograms;
  histograms.push_back({"_TotalTime", "Total time"});

  // Provide output to user for progress status
  //  std::cout << "main() processing algorithms : " << algorithms << std::endl;

  // Loop over histograms
  for (unsigned int histogram = 0; histogram < histograms.size(); ++histogram) {

    std::string xaxis = histograms.at(histogram).dname + " [ms]";
    std::string yaxis = "Entries";

    // Loop over input algorithms
    for (unsigned int algorithm = 0; algorithm < algorithms.size(); ++algorithm) {

      std::cout << "main() processing algorithm : " << algorithms[algorithm] << std::endl;

      TCanvas* c1 = new TCanvas(label("canvas-%d",static_cast<int>(histogram)).c_str(),"histogram",800,600);
      c1->cd();
            
      double const x1 = 0.15;
      double const x2 = 0.25;
      double const y1 = 0.83;
      double const y2 = 0.90;

      Legend legend(x1, x2, y1, y2);
     

      std::string histname = algorithms.at(algorithm) + histograms.at(histogram).fname;

      TH1* testhist = 0 ;
      testtimers->GetObject(histname.c_str(), testhist);
      if (testhist == 0 ) {
        std::cerr << "main(): can not find hist " << histname << " in test file" << std::endl;
        continue;
      }

      TH1* refhist = 0 ;
      reftimers->GetObject(histname.c_str(),refhist);
      if (refhist == 0 ) {
        std::cerr << "main(): can not find hist " << histname << " in ref file" << std::endl;
        continue;
      }

      testhist->GetYaxis()->SetTitleOffset(1.5);
      refhist->GetYaxis()->SetTitleOffset(1.5);
      testhist->GetXaxis()->SetTitle(xaxis.c_str());
      testhist->GetYaxis()->SetTitle(yaxis.c_str());
      refhist->GetXaxis()->SetTitle(xaxis.c_str());
      refhist->GetYaxis()->SetTitle(yaxis.c_str());

      Plots plots;

      plots.push_back(Plotter(testhist, refhist, algorithms.at(algorithm)));

      if (histograms.at(histogram).fname == "_TotalTime") {
        testhist->SetTitle("");
        refhist->SetTitle("");
        c1->SetLogy(true);
        plots.xrange();
        plots.Max(5);
      }
      else {
        c1->SetLogy(false);
      }

      plots.Draw( legend, true );

      std::string plotname = dir + key + algorithms.at(algorithm) +
                              histograms.at(histogram).fname + tag;

      plots.back().Print(plotname+tag+".pdf");
      plots.back().Print(plotname+tag+".png");

      delete c1;
    }
  }

  delete testtimers;
  delete ftest;
  delete reftimers;
  delete fref;

  return 0;
}
