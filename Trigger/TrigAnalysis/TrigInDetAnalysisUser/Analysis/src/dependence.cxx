/**
 **     @file    dependence.cxx
 **
 **     @author  berare gokturk
 **     @date    Mon 17 May 2021 20:02:26 CET 
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/

#include <string>
#include <iostream>

#include "TrigInDetAnalysis/Efficiency.h"
#include "ReadCards.h"


#include "TH1F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "TPad.h"
#include "TCanvas.h"

#include "TStyle.h"

#include "AtlasStyle.h"

int usage( const std::string& err_msg="", int status=0 ) { 
  
  std::string name = "dependence";

  if ( err_msg!="" ) std::cerr << err_msg << std::endl;
  
  std::ostream& s = std::cout;
  
  s << "Usage: " << name << "\t [OPTIONS] \n\n"; 
  s << "\t" << " plots comparison histograms"; 
  s << " - compiled on " << __DATE__ << " at " << __TIME__ << "\n\n";
  s << "Options: \n";
  s << "    -c,  --config   value \t configure which histograms to plot from config file,\n\n";
  s << "    -f,  --fit            \t fit a pol2 to the efficiencies\n\n";
  s << "    -as, --atlasstyle    \t usethe atlas style\n\n";

  s << "    -h,  --help              \t this help\n";
  
  s << std::endl;

  return status;
} 


void add_to_bin( double d, TH1* h, TH1* h0 ) { 
 
  int ibin = h->FindBin(d);
  if ( ibin<1 || ibin>h->GetNbinsX() ) return;

  double v = 0;
  for ( int i=1 ; i<=h0->GetNbinsX() ; i++ ) v += h0->GetBinContent(i);
  h->SetBinContent( ibin, v ); 
} 


bool doFit = false;

void efficiency( std::vector<double>& bins, std::vector<double>& values, const std::vector<std::string>& files, 
		 const std::string& histname, const std::string& plotname ) {
 
  if ( values.size() != files.size() ) { 
    std::cerr << "number of values (" << values.size() << ") and files (" << files.size() << ") do not match" << std::endl;
    std::exit(-1);
  }

  TH1F* hd  = 0;
  TH1F* hn  = 0;

  if ( bins.size()==3 ) { 
    hd  = new TH1F( "denominator",  ";roi z width [mm];efficiency ", int(bins[0]), bins[1], bins[2] ); 
    hn  = new TH1F( "numerator",    ";roi z width [mm];efficiency ", int(bins[0]), bins[1], bins[2] ); 
  }
  else if ( bins.size()>3 ) { 
    hd  = new TH1F( "denominator",  ";roi z width [mm];efficiency ", bins.size()-1, &bins[0] );
    hn  = new TH1F( "numerator",    ";roi z width [mm];efficiency ", bins.size()-1, &bins[0] );
  }
  else { 
    std::exit( usage( "unusable binning", -1) );  
  }

  std::cout << "looping over files ..." << std::endl;

  for ( size_t i=0 ; i<files.size() ; i++ ) { 
    
    //    std::string s = "run" + std::to_string(i) + "/data-hists-offline" + ".root"; 
    std::string s = files[i];
    
    std::cout << "file: "<< s << "\tvalue: "<< values[i] << std::endl; 

    //    std::cout << "\t hist: " << histname << std::endl; 
    
    TFile* f = new TFile( s.c_str() );
    
    if ( f==0 ) { 
      std::cerr << "could not open file" << s << std::endl;
      std::exit( -1 );
    }

    TH1F* he_n = (TH1F*)f->Get( (histname+"_n").c_str() );
    TH1F* he_d = (TH1F*)f->Get( (histname+"_d").c_str() );
    
    if ( he_n==0 ) { 
      std::cerr << "histogram " << histname+"_n" << "could not be retrieved" << std::endl;
      continue;
    }

    if ( he_d==0 ) { 
      std::cerr << "histogram " << histname+"_d" << "could not be retrieved" << std::endl;
      continue;
    }

    //    std::cout << "\t he_n: " << he_n << std::endl;
    //    std::cout << "\t he_d: " << he_d << std::endl;
    
    add_to_bin( values[i], hn, he_n );
    add_to_bin( values[i], hd, he_d );
  
    delete f;
  }

  double scale_eff = 1;
  Efficiency e( hn, hd, "", scale_eff );

  TGraphAsymmErrors* tgtest = e.Bayes(scale_eff);

  TCanvas* c2 = new TCanvas( plotname.c_str(), "eff", 10, 10, 1000, 500 );
  c2->cd();

  TH1F* h = e.Hist();

  h->SetMarkerStyle(20);
  h->SetLineColor(h->GetMarkerColor());

  h->GetYaxis()->SetRangeUser(0.90,1.02);

  tgtest->SetMarkerStyle(20);
  tgtest->SetMarkerColor(h->GetMarkerColor());
  tgtest->SetLineColor(h->GetMarkerColor());

  if ( doFit ) { 
    h->Fit( "pol2" );
    TF1* f1 = (TF1*)h->GetListOfFunctions()->FindObject("pol2");
    f1->SetLineWidth(1);
  }


  h->Draw("e1");
  tgtest->Draw("samep");

  gPad->SetLogx(true);

  gPad->Print( plotname.c_str() );

  delete hd;
  delete hn;
  delete c2;
  

}




int main( int argc, char** argv ) { 

  /// configuration file ...

  std::string config_file = "";

  bool atlasstyle = false;

  /// read command line arguments ...

  for ( int i=1 ; i<argc ; i++ ) { 
    std::string arg = argv[i];

    if ( arg=="-h" || arg=="--help" ) return usage(); 
    else if ( arg=="-f" || arg=="--fit" ) doFit = true; 
    else if ( arg=="-as" || arg=="--atlasstyle" ) atlasstyle = true; 
    else if ( arg=="-c" ) { 
      if ( ++i<argc ) config_file = argv[i];
      else return usage( "no config file provided", -1 );
    }
  }

  if ( config_file == "" ) return usage( "no config file provided", -1 );

  if ( atlasstyle ) SetAtlasStyle();

  gStyle->SetOptStat(0);
  gStyle->SetErrorX(0);

  /// open configuration file ...
  
  ReadCards config(config_file);


  /// get the list of efficiencies to cteate ...

  std::vector<std::string> plots = config.GetStringVector("plots");

  /// get the bins for the efficiency, and the values to be used for each file  ...

  std::vector<double> bins   = config.GetVector("bins");

  std::vector<double> values = config.GetVector("values");

  /// and the list of files ...

  std::vector<std::string> files = config.GetStringVector("files");

  
  /// off we go ...

  std::cout << "looping over plots ..." << std::endl;

  for ( size_t i=0 ; i<plots.size() ; i+=2 ) { 
    
    std::string hist     = plots[i];
    std::string plotname = plots[i+1];

    std::cout << "plot: " << plotname << "\t" << hist << std::endl;

    efficiency( bins, values, files, hist, plotname );
   
  }

  return 0;

}
