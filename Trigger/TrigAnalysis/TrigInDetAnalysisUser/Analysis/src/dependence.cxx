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
#include <cstdlib>
#include <cmath>

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
#include "DrawLabel.h"



int usage( const std::string& err_msg="", int status=0 ) { 
  
  std::string name = "dependence";

  if ( err_msg!="" ) std::cerr << err_msg << std::endl;
  
  std::ostream& s = std::cout;
  
  s << "Usage: " << name << "\t [OPTIONS] \n\n"; 
  s << "\t" << " plots comparison histograms"; 
  s << " - compiled on " << __DATE__ << " at " << __TIME__ << "\n\n";
  s << "Options: \n";
  s << "    -c,  --config   value \t configure which histograms to plot from config file,\n\n";
  s << "    -f,  --fit            \t fit a pol2 to the efficiencies\n\\n";
  s << "    -lx, --logx           \t drawwith log x axis\n";
  s << "    -as, --atlasstyle    \t usethe atlas style\n\n";

  s << "    -h,  --help              \t this help\n";
  
  s << std::endl;

  return status;
} 



bool doFit  = false;
bool doLogx = false;

double ptmax = 0;

void add_to_bin( double d, TH1* h, TH1* h0 ) { 
 
  int ibin = h->FindBin(d);
  if ( ibin<1 || ibin>h->GetNbinsX() ) return;

  if ( ptmax ) { 
    double v = 0;
    for ( int i=1 ; i<=h0->GetNbinsX() ; i++ ) if ( h0->GetBinCenter(i)<ptmax ) v += h0->GetBinContent(i);
    h->SetBinContent( ibin, v ); 
  }
  else { 
    double v = 0;
    for ( int i=1 ; i<=h0->GetNbinsX() ; i++ ) v += h0->GetBinContent(i);
    h->SetBinContent( ibin, v ); 
  } 
}


std::string runfit( TH1F* h ) { 

  double chi2dof = 0;
  double chi2 = 0;
  int    ndof = 0;

  std::string fitf = "";

  TF1* hfit = new TF1( "hyper", "[0]-[3]*( (x-[1]) + sqrt( (x-[1])*(x-[1]) + 4*[2]*[2] ) )/(2*[2])", 0, 5 ); 

  hfit->SetParameter(0, 0.95);
  hfit->SetParameter(1, 1);
  hfit->SetParameter(2, 1);
  hfit->SetParameter(3, 1);

  std::string fitname[4] = { "pol0", "pol1", "hyper", "pol2" };
  
  for ( int ifit=0 ; ifit<4 ; ifit++ ) { 
    
    h->Fit( fitname[ifit].c_str() );
    TF1* f1 = (TF1*)h->GetListOfFunctions()->FindObject( fitname[ifit].c_str() );
    
    f1->SetLineWidth(1);
    f1->SetLineColor(kRed);
    
    chi2 = f1->GetChisquare();
    ndof = f1->GetNDF();
    
    chi2dof = chi2/ndof;
    
    fitf = fitname[ifit];

    char flabel[256];

    std::sprintf( flabel, "%s: #chi^{2}/dof = %6.2lf/%d", fitf.c_str(), chi2, ndof );
    
    std::string chi2label = flabel;
    
    if ( chi2dof < 1.5 ) return chi2label;
    
  }
	
  return "";

}



void efficiency( std::vector<double>& bins, std::vector<double>& values, const std::vector<std::string>& files, 
		 const std::string& histname, const std::string& tplotname, const std::string& label="" ) {


  std::string plotname = tplotname;
  std::string labels  = ";value;mean";

  size_t pos = plotname.find(';');

  if ( pos!=std::string::npos ) { 
    labels   = plotname.substr( pos, plotname.size() );
    plotname = plotname.substr( 0, pos );
    std::cout << "plotname: " << plotname << "\tlabels: " << labels << std::endl;  
  } 

 
  if ( values.size() != files.size() ) { 
    std::cerr << "number of values (" << values.size() << ") and files (" << files.size() << ") do not match" << std::endl;
    std::exit(-1);
  }

  TH1F* hd  = 0;
  TH1F* hn  = 0;

  if ( bins.size()==3 ) { 
    hd  = new TH1F( "denominator", labels.c_str(), int(bins[0]), bins[1], bins[2] ); 
    hn  = new TH1F( "numerator",   labels.c_str(), int(bins[0]), bins[1], bins[2] ); 
  }
  else if ( bins.size()>3 ) { 
    hd  = new TH1F( "denominator",  labels.c_str(), bins.size()-1, &bins[0] );
    hn  = new TH1F( "numerator",    labels.c_str(), bins.size()-1, &bins[0] );
  }
  else { 
    std::exit( usage( "unusable binning", -1) );  
  }

  std::cout << "looping over files ..." << std::endl;

  for ( size_t i=0 ; i<files.size() ; i++ ) { 
    
    std::string s = files[i];
    
    std::cout << "file: "<< s << "\tvalue: "<< values[i] << std::endl; 

    TFile* f = new TFile( s.c_str() );
    
    if ( f==0 ) { 
      std::cerr << "could not open file" << s << std::endl;
      std::exit( -1 );
    }

    /// fetch the numerator and denominator histograms
    /// from the efficiency

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

    add_to_bin( values[i], hn, he_n );
    add_to_bin( values[i], hd, he_d );

    delete he_n;
    delete he_d;

    delete f;
  }

  double scale_eff = 1;
  Efficiency e( hn, hd, "", scale_eff );

  TGraphAsymmErrors* tgtest = e.Bayes(scale_eff);

  TCanvas* c2 = new TCanvas( plotname.c_str(), "eff", 10, 10, 700, 500 );
  c2->cd();

  TH1F* h = e.Hist();

  h->SetMarkerStyle(20);
  h->SetLineColor(h->GetMarkerColor());

  h->GetYaxis()->SetRangeUser(0.85,1.02);
  h->GetXaxis()->SetRangeUser(0.4,1.7);

  tgtest->SetMarkerStyle(20);
  tgtest->SetMarkerColor(h->GetMarkerColor());
  tgtest->SetLineColor(h->GetMarkerColor());


  std::string chi2label = "";

  if ( doFit ) chi2label = runfit( h );

  h->GetXaxis()->SetMoreLogLabels(true);

  h->Draw("e1");
  
  tgtest->Draw("samep");

  if ( doLogx ) gPad->SetLogx(true);

  DrawLabel( 0.2, 0.25, label );
  if ( chi2label!="" ) DrawLabel( 0.2, 0.21, chi2label );


  gPad->Print( plotname.c_str() );

  delete hd;
  delete hn;
  delete c2;
  

}





void mean( std::vector<double>& bins, std::vector<double>& values, const std::vector<std::string>& files, 
	   const std::string& histname, const std::string& tplotname, const std::string& label="" ) {
 
  if ( values.size() != files.size() ) { 
    std::cerr << "number of values (" << values.size() << ") and files (" << files.size() << ") do not match" << std::endl;
    std::exit(-1);
  }

  TH1F* h  = 0;
  

  std::string plotname = tplotname;
  std::string labels  = ";value;mean";

  size_t pos = plotname.find(';');

  if ( pos!=std::string::npos ) { 
    labels   = plotname.substr( pos, plotname.size() );
    plotname = plotname.substr( 0, pos );
    std::cout << "plotname: " << plotname << "\tlabels: " << labels << std::endl;  
  } 

  
  if ( bins.size()==3 ) { 
    h  = new TH1F( "mean",  labels.c_str(), int(bins[0]), bins[1], bins[2] ); 
  }
  else if ( bins.size()>3 ) { 
    h  = new TH1F( "mean", labels.c_str(), bins.size()-1, &bins[0] );
  }
  else { 
    std::exit( usage( "unusable binning", -1) );  
  }

  std::cout << "looping over files ..." << std::endl;

  for ( size_t i=0 ; i<files.size() ; i++ ) { 

    std::string s = files[i];
    
    TFile* f = new TFile( s.c_str() );
    
    if ( f==0 ) { 
      std::cerr << "could not open file" << s << std::endl;
      std::exit( -1 );
    }

    ///    get the histogram from which we wish to extract the mean

    TH1F* he = (TH1F*)f->Get( histname.c_str() );
    
    if ( he==0 ) { 
      std::cerr << "histogram " << histname << "could not be retrieved" << std::endl;
      continue;
    }

    std::cout << "\t he: " << he << std::endl;
    
    int ibin = h->FindBin(values[i]);
    if ( ibin<1 || ibin>h->GetNbinsX() ) continue;


    std::cout << "bin:  " << ibin << std::endl;
    std::cout << "mean: " << he->GetMean() << " +- " << he->GetMeanError() << std::endl;
    

    h->SetBinContent( ibin,  he->GetMean() );
    h->SetBinError( ibin,  he->GetMeanError() );

    /// technically we do not need to delete the histgram, as it belongs 
    /// to the file so will be deleted when the file is Closed, before
    /// before it is deleted    
    delete he;
    delete f;
  }

  //  TCanvas* c = new TCanvas( plotname.c_str(), "eff", 10, 10, 1000, 500 );
  TCanvas* c = new TCanvas( plotname.c_str(), "eff", 10, 10, 700, 500 );
  c->cd();

  h->SetMarkerStyle(20);
  h->SetLineColor(h->GetMarkerColor());

  std::string chi2label = "";

  if ( doFit ) chi2label = runfit( h );

  h->GetXaxis()->SetMoreLogLabels(true);

  h->Draw("e1");

  if ( doLogx ) gPad->SetLogx(true);

  DrawLabel( 0.2, 0.25, label );
  if ( chi2label!="" ) DrawLabel( 0.2, 0.21, chi2label );

  gPad->Print( plotname.c_str() );

  delete h;
  delete c;
  
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
    else if ( arg=="-lx" || arg=="--logx" ) doLogx = true; 
    else if ( arg=="-c" ) { 
      if ( ++i<argc ) config_file = argv[i];
      else return usage( "no config file provided", -1 );
    }
  }

  if ( config_file == "" ) return usage( "no config file provided", -1 );

  if ( atlasstyle ) SetAtlasStyle();

  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);

  gStyle->SetOptStat(0);
  gStyle->SetErrorX(0);

  /// open configuration file ...
  
  ReadCards config(config_file);

  bool do_mean       = false;
  bool do_efficiency = false;

  if ( config.isTagDefined("pTmax") ) ptmax = config.GetValue( "pTmax" );

  if ( config.isTagDefined("mean") )       do_mean       = ( config.GetValue( "mean")>0 ? true : false );
  if ( config.isTagDefined("efficiency") ) do_efficiency = ( config.GetValue( "efficiency")>0 ? true : false );

  //  void (*function)( std::vector<double>& bins, std::vector<double>& values, const std::vector<std::string>& files, 
  //		    const std::string& histname, const std::string& plotname ) = 0;

  std::vector<void (*)( std::vector<double>& bins, std::vector<double>& values, const std::vector<std::string>& files, 
			const std::string& histname, const std::string& plotname, const std::string& label ) > function;

  std::vector<std::vector<std::string> > plots;

  std::vector<std::vector<std::string> > files;

  if ( do_efficiency ) { 
    function.push_back( efficiency );
    files.push_back( config.GetStringVector("efiles") );
    plots.push_back( config.GetStringVector("eplots") );
  }
   
  if ( do_mean ) {
    function.push_back( mean );
    files.push_back( config.GetStringVector("mfiles") );
    plots.push_back( config.GetStringVector("mplots") );
  }

  if ( function.empty() ) return 0;

  /// get the list of efficiencies to cteate ...


  /// get the bins for the efficiency, and the values to be used for each file  ...

  std::vector<double> bins   = config.GetVector("bins");

  std::vector<double> values = config.GetVector("values");

  /// and the list of files ...

  


  // output plot name 

  //  std::string plotname = config.GetString("plotname");

  
  /// off we go ...

  std::cout << "looping over plots ..." << std::endl;

  for ( size_t j=function.size() ; j-- ; ) { 

    for ( size_t i=0 ; i<plots[j].size() ; i+=3 ) { 
      
      std::string hist     = plots[j][i];
    
      std::string plotname = plots[j][i+1];
    
      std::string label = plots[j][i+2];
     
      std::cout << "plot: " << plotname << " :\t" << hist << "\tlabel: " << label << std::endl;
      
      function[j]( bins, values, files[j], hist, plotname, label );
     
    }

  }

  return 0;

}
