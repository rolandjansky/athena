//
//   @file    comparitor.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: comparitor.cxx, v0.0   Fri 12 Oct 2012 13:39:05 BST sutt $


#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h> 

#include <iostream>
#include <string>
#include <vector>

#include "TrigInDetAnalysis/Efficiency.h"
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



int usage(const std::string& name, int status) { 
  std::ostream& s = std::cout;
  s << "Usage: " << name << " [OPTIONS]  test.root reference.root    chain1 chain2 chain2 ...\n\n"; 
  s << "  TIDA \'" << name << "\' plots comparison histograms\n\n"; 
  //  s << "Configuration: \n";
  //  s << "    -o filename   \tname of output grid (filename required)\n\n";
  s << "Options: \n";
  s << "    -t, --tag value   \t appends tag 'value' to the end of output plot names, \n";
  s << "    -k, --key value   \t prepends key 'value' to the from of output plots name, \n";
  s << "    -d, --dir value   \t creates output files into directory, \"value\" \n";
  s << "    -e, --efficiencies\t make test efficiencies with respect to reference \n";
  s << "    -h, --help        \t this help\n";
  //  s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  //  s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
  s << std::endl;
  return status;
}




int main(int argc, char** argv) { 

  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.05);
  
  if ( argc<4 ) return usage(argv[0], -1);


  std::string tag = "";
  std::string key = "";

  std::string dir = "";

  TFile* _ftest = 0;
  TFile* _fref  = 0;

  bool make_ref_efficiencies = false;

  std::vector<std::string> chains;
  for(int i=1; i<argc; i++){
    std::string arg  = argv[i];

    if ( arg=="-h" || arg=="--help" ) { 
       return usage(argv[0], 0);
    }
    else if ( arg=="-t" || arg=="--tag" ) { 
      if ( ++i<argc ) tag=std::string("-")+argv[i];
      else return usage(argv[0], -1);
    }
    else if ( arg=="-k" || arg=="--key" ) { 
      if ( ++i<argc ) key=argv[i];
      else return usage(argv[0], -1);
    }
    else if ( arg=="-d" || arg=="--dir" ) { 
      if ( ++i<argc ) dir=argv[i];
      else return usage(argv[0], -1);
    }
    else if ( arg=="-e" || arg=="--efficiencies" ) { 
      make_ref_efficiencies = true;
    }
    else { 
      if ( _ftest==0 ) { 
	if ( exists(arg) ) _ftest = new TFile( arg.c_str() );
	else { 
	  std::cerr << "main(): test file " << arg << " does not exist" << std::endl;
	  return -1;
	}	    
      }
      else if ( _fref==0 ) { 
	if ( exists(arg) ) _fref = new TFile( arg.c_str() );
	else { 
	  if ( _ftest ) delete _ftest;
	  std::cerr << "main(): ref file  " << arg << " does not exist" << std::endl;
	  return -1;
	}	    
      }
      else { 
	std::string chain = arg;
	replace ( chain, ":", "_" );
	replace ( chain, ";", "_" );
	chains.push_back(chain);
      }
    }
  }


  /// get release data

  std::vector<std::string> release_data;

  TTree*   dataTree    = (TTree*)_ftest->Get("dataTree");
  TString* releaseData = new TString("");
  
  if ( dataTree ) { 
    dataTree->SetBranchAddress( "ReleaseMetaData", &releaseData);
    
    for (unsigned int i=0; i<dataTree->GetEntries() ; i++ ) {
      dataTree->GetEntry(i);      
      release_data.push_back( releaseData->Data() );
      std::cout << "main() release data: " << release_data.back() << " " << *releaseData << std::endl;
    }
  }

  if ( release_data.size()>0 ) { 
    if ( release_data.size()>1 ) std::cerr << "main() more than one release - using only the first" << std::endl;  

    //    std::cout << "release: " << chop(release_data[0], " " ) << std::endl;
    //    std::cout << "release: " << chop(release_data[0], " " ) << std::endl;
    
    release += "  (" + chop(release_data[0], " " );
    release += " " + chop(release_data[0], " " ) + ")";
  }


  // Make output directory                                                                                                                           
  if (dir != "") {
    dir += "/";
    if ( mkdir( dir.c_str(), 0777 ) ) std::cerr << "main() couldn't create directory " << dir << std::endl;
    else                              std::cerr << "main() output will be sent to directory " << dir << std::endl; 
  }

  TFile& ftest = *_ftest;
  TFile& fref  = *_fref;

  
  std::vector<std::string> chainnames;

  chainnames.resize(chains.size());
  chainnames.clear();

  int NeventTest=0;
  int NeventRef=0;
  TH1D* htestev = (TH1D*)ftest.Get("event") ;
  TH1D* hrefev  = (TH1D*)fref.Get("event") ;

  NeventTest = htestev->GetEntries();
  NeventRef  = hrefev->GetEntries();

  const int Nhistos = 36;
  std::string histos[Nhistos] = { 
    "pT",
   /// efficiencies
    "pT_eff",       
    "eta_eff", 
    "phi_eff", 
    "d0_eff", 
    "a0_eff", 
    "z0_eff", 
    "eff_vs_mu", 
    "roi_dphi_eff",
    "roi_deta_eff",
    "roi_dR_eff",
    /// standard residuals
    "ipT_res", 
    "eta_res", 
    "phi_res", 
    "z0_res",
    /// residuals vs track parameters
    "rd0_vs_pt/sigma",
    "rd0_vs_signed_pt/sigma",
    "rd0_vs_ABS_pt/sigma",
    "rd0_vs_eta/sigma",
    "ript_vs_pt/sigma",
    "reta_vs_pt/sigma",
    "reta_vs_eta/sigma",
    "rd0_vs_eta/sigma",
    "ript_vs_eta/sigma",
    "rzed_vs_eta/sigma",
    "rzed_vs_pt/sigma",
    "rzed_vs_signed_pt/sigma",
    "rzed_vs_ABS_pt/sigma",
    "rzed_vs_zed/sigma",
    "reta_vs_ipt/sigma",
    "rphi_vs_ipt/sigma",
    "rzed_vs_ipt/sigma",
    "ript_vs_ipt/sigma",
    "rpt_vs_ipt/sigma",
    "rd0_vs_ipt/sigma",
    "ntracks_rec"
  };
  
  std::string histonames[Nhistos] = { 
    "P_{T}",
    "Efficiency P_{T}", 
    "Efficiency #eta", 
    "Efficiency #phi", 
    "Efficiency d0", 
    "Efficiency a0", 
    "Efficiency z0", 
    "Efficiency <#mu>", 
    "Efficiency #Delta#phi(RoI)",
    "Efficiency #Delta#eta(RoI)",
    "Efficiency #DeltaR(RoI)",
    
    "Residual 1/P_{T}", 
    "Residual #eta", 
    "Residual #phi", 
    "Residual z0",
    
    "Residual d vs P_{T}",
    "Residual d vs signed P_{T}",
    "Residual d vs absolute P_{T}",
    "Residual d vs #eta",
    "Residual 1/P_{T} vs P_{T}",
    "Residual #eta vs P_{T}",
    "Residual #eta vs #eta",
    "Residual d vs #eta",
    "Residual 1/P_{T} vs #eta",
    "Residual z vs #eta",
    "Residual z vs P_{T}",
    "Residual z vs signed P_{T}",
    "Residual z vs absolute P_{T}",
    "Residual z vs z",
    "Residual #eta vs 1/P_{T}",
    "Residual #phi vs 1/P_{T}",
    "Residual z vs 1/P_{T}",
    "Residual 1/P_{T} vs 1/P_{T}",
    "Residual P_{T} vs 1/P_{T}",
    "Residual d vs 1/P_{T}",
    "Number of reconstructed tracks"
  }; 

  const int Nhistos2D = 2;

  std::string histos2D[Nhistos2D] = {
    "eta_phi_rec",
    "phi_d0_rec",
  };

  std::string histonames2D[Nhistos2D] = {
    "#phi vs #eta",
    "d_{0} vs #phi",
  };


  //  std::cout << "size of chains " << chains.size() << std::endl;

  std::cout << "main() processing chains : " << chains << std::endl; 
  
  if ( chains.empty() ) return 0;
  for ( unsigned int j=0; j<chains.size(); j++)  {

      gStyle->SetPadRightMargin(0.05);
      gStyle->SetPadTopMargin(0.05);

      Int_t MyPalette[100];
      Double_t r[9]    = { 0.,  0.5,   0.80,  0.00,  0.20,  0.0,   1.0,  1.0,   1.0};
      Double_t g[9]    = { 0.,  0.,    0.00,  0.00,  0.80,  0.8,   0.0,  1.0,   1.0};
      Double_t b[9]    = { 0.,  0.5,   0.80,  0.80,  1.00,  0.0,   0.0,  0.0,   0.8};
      Double_t stop[9] = { 0.,  0.125, 0.25,  0.375, 0.50,  0.625, 0.75, 0.875, 1.0};

      Int_t FI = TColor::CreateGradientColorTable(9, stop, r, g, b, 100);

      for (int ii=0;ii<100;ii++) MyPalette[ii] = FI+ii;

      if ( chains[j]=="FTK_TrackParticle"){

          for ( int i=0 ; i<Nhistos2D ; i++ ) {

	    if ( contains( histos2D[i],"eta_phi_rec") ) gStyle->SetPalette(100, MyPalette);
              else   gStyle->SetPalette(1);
           
              gStyle->SetOptStat(0);
              TH2F* htest2D = (TH2F*)ftest.Get((chains[j]+"/"+histos2D[i]).c_str()) ;
              TH2F* href2D  = (TH2F*)fref.Get((chains[j]+"/"+histos2D[i]).c_str()) ;
           
	      std::string plotname;
              plotname += "FTK_";
              plotname += histos2D[i].c_str();
              plotname += ".png";
              
	      TCanvas* c1 = new TCanvas(label("canvas2D-%d",i).c_str(),"histogram",1200,500);
              c1->Divide(2,1);
              c1->cd(1);
              
	      std::string title;
              title+=histonames2D[i]+" test";
             
	      htest2D->SetTitle(title.c_str());
              htest2D->Scale(1./NeventTest);
              htest2D->Draw("COLZ");
	      

	      c1->cd(2);
             
	      std::string title2;
              title2+=histonames2D[i]+" reference";
              href2D->SetTitle(title2.c_str());
              href2D->Scale(1./NeventRef);
              //gPad->SetLogz();                                                                                         
              href2D->Draw("COLZ");
              replace(plotname, "/", "_");
              c1->Print((dir+plotname).c_str());
              delete c1;
          }
      }
  }


  for ( int i=0 ; i<Nhistos ; i++ ) {

    //    std::cout << "main() processing histos[" << i <<  "] " << (i<10 ? " " : "" ) << (chains[0]+"/"+histos[i]) << std::endl;
    std::cout << "main() processing histos[" << i <<  "] " << (i<10 ? " " : "" ) << histos[i] << std::endl;

#if 0
    //  Quick tests for new histograms
    TH1D* hNewRef  = (TH1D*)fref.Get((chains[0]+"/"+histos[i]).c_str());
    if ( hNewRef==0 ) {
      std::cout << "       no ref histogram :    " << (chains[0]+"/"+histos[i]) << " - skipping" << std::endl;
      continue;
    }

    TH1D* hNewTest = (TH1D*)ftest.Get((chains[0]+"/"+histos[i]).c_str());
    if ( hNewTest==0 ) {
      std::cout << "       no test histogram :   " << (chains[0]+"/"+histos[i]) << " - skipping" << std::endl;
      continue;
    }
#endif

    //    std::vector<Plotter> plots;

    Plots plots_eff;
    plots_eff.clear();

    Plots plots;
    plots.clear();
  
    TCanvas* c1 = new TCanvas(label("canvas-%d",i).c_str(),"histogram",800,600);
    c1->cd();

    Legend legend;
    // efficiencies or residuals?
    if ( contains(histos[i],"eff") ) legend = Legend( 0.15, 0.25, 0.4-chains.size()*0.07, 0.40 );
    else                             legend = Legend( 0.15, 0.25, 0.9-chains.size()*0.07, 0.90 );

    Legend legend_eff;
    // efficiencies or residuals?
    if ( contains(histos[i],"eff") ) legend_eff = Legend( 0.15, 0.25, 0.4-chains.size()*0.07, 0.40 );
    else                             legend_eff = Legend( 0.15, 0.25, 0.9-chains.size()*0.07, 0.90 );


   
    std::vector<std::string> Mean;
    std::vector<std::string> RMS;
  
    Mean.clear();
    RMS.clear();

    std::vector<std::string> MeanRef;
    std::vector<std::string> RMSRef;
  
    //    int colours[6] = { 1, 2, 4, 6, 7, 8 };

    MeanRef.clear();
    RMSRef.clear();

    std::string plotname; 

    std::string xaxis = "";
    std::string yaxis = "";

    /// work out axis labels
    /// x axis
    if ( contains(histos[i],"vs_pt") || histos[i]=="pT")  xaxis = "Offline P_{T} [GeV]";
    if ( contains(histos[i],"vs_signed_pt") )             xaxis = "Signed Offline P_{T} [GeV]";
    if ( contains(histos[i],"vs_absolute_pt") )           xaxis = "Absolute Offline P_{T} [GeV]";
    if ( contains(histos[i],"vs_eta") )                   xaxis = "Offline #eta"; 
    if ( contains(histos[i],"vs_zed") )                   xaxis = "Offline z_{0} [mm]"; 
    if ( contains(histos[i],"a0" ) && 
	 contains(histos[i],"vs" )==std::string::npos )                      xaxis = "Offline a_{0} [mm]"; 
    if ( contains(histos[i],"d0") && 
	 contains(histos[i],"vs" )==std::string::npos)                       xaxis = "Offline d_{0} [mm]"; 
    if ( contains(histos[i],"mu") )                       xaxis = "<#mu>"; 
    if ( contains(histos[i],"vs_ipt") )                   xaxis = "true 1/P_{T} [GeV^{-1}]";
	 

    if ( contains(histos[i],"eff") )  { 
      yaxis = "Efficiency [%]"; 
      if ( contains(histos[i],"z0") ) xaxis = "z_{0} [mm]";
      if ( contains(histos[i],"pT") ) xaxis = "P_{T} [GeV]";
      if ( contains(histos[i],"d0") ) xaxis = "d_{0} [mm]";
      if ( contains(histos[i],"a0") ) xaxis = "a_{0} [mm]";
    }
    if ( contains(histos[i],"ript") ) yaxis = "rms_{95} 1/P_{T} residual [GeV^{-1}]"; 
    if ( contains(histos[i],"reta") ) yaxis = "rms_{95} #eta residual"; 
    if ( contains(histos[i],"rzed") ) yaxis = "rms_{95} z_{0} residual [mm]"; 
    if ( contains(histos[i],"rd0") )  yaxis = "rms_{95} d_{0} residual [mm]"; 
   
    int mean_power = 0;
    int rms_power  = 0;
    bool power_set = false;;
    

    bool uselogx = false;
 
    for ( unsigned int j=0; j<chains.size(); j++)  {

      std::cout << "       processing chain[" << j << "]   " << chains[j] << std::endl;
            
      TH1D* htest = (TH1D*)ftest.Get((chains[j]+"/"+histos[i]).c_str()) ;
      TH1D* href  = (TH1D*)fref.Get((chains[j]+"/"+histos[i]).c_str()) ;

      TH1F* htestnum = 0;
      TH1F* hrefnum  = 0;
      
      if ( make_ref_efficiencies ) { 
	if ( htest && href ) { 
	  if ( contains( std::string(htest->GetName()), "_eff" ) ) {

	    htestnum = (TH1F*)ftest.Get((chains[j]+"/"+histos[i]+"_n").c_str()) ;
	    hrefnum  = (TH1F*)fref.Get((chains[j]+"/"+histos[i]+"_n").c_str()) ;
	    
	    //	    std::cout << "numerator histos " << htestnum << " " << hrefnum << std::endl;

	  }
	}
      }



      if ( htest==0 ) { 
	std::cout << "       no test histogram :   " << (chains[j]+"/"+histos[i]) << std::endl;
	continue;
      }
      
      if ( href==0 ) { 
	std::cout << "       no ref histogram :    " << (chains[j]+"/"+histos[i]) << std::endl;
	continue;
      }
      
      if ( xaxis!="" ) {
	htest->GetYaxis()->SetTitleOffset(1.5);
	href->GetYaxis()->SetTitleOffset(1.5);
	htest->GetXaxis()->SetTitle(xaxis.c_str());
	htest->GetYaxis()->SetTitle(yaxis.c_str());
	href->GetXaxis()->SetTitle(xaxis.c_str());
	href->GetYaxis()->SetTitle(yaxis.c_str());
      }

      //to stop negative y axis because of error bars, need 0.01 to have log scale
      // JK - change this to 0.005 for d0 only?
      if ( contains(histos[i],"vs") ) { 
	if ( !contains(histos[i],"eff") ) { 
	  if ( contains(histos[i],"vs_zed") ) { 
	    htest->GetXaxis()->SetRangeUser(-200,200);
	    href->GetXaxis()->SetRangeUser(-200,200);
	  }
	  htest->SetMinimum(0.01);
	  href->SetMinimum(0.01);
	  if ( contains(histos[i],"vs_pt") ) { 
	    htest->GetXaxis()->SetRangeUser(0,100);
	    href->GetXaxis()->SetRangeUser(0,100);
	    htest->SetMinimum(0.001);
	    href->SetMinimum(0.001);
	  }
	}
	else { 
	  //to stop negative y axis because of error bars, need 0.01 to have log scale
	  htest->SetMinimum(0.01);
	  href->SetMinimum(0.01);
	}
      }
      
      if ( contains(histos[i],"ntracks") ) {
	htest->Rebin(10);
        href->Rebin(10);
        htest->Sumw2();
        href->Sumw2();
        htest->Scale(1./NeventTest);
        href->Scale(1./NeventRef);
      }

      // ADDED BY JK - custom d0 residual lower limit. Precursor to implementing automatic lower range finder.
      if ( contains(histos[i],"rd0") ) {
	htest->SetMinimum(0.005);
	href->SetMinimum(0.005);
      }
      
      //      if ( contains(histos[i],"res") ) { 
      //	htest->SetMinimum(0.01);
      //	href->SetMinimum(0.01);
      //      }


      if ( contains(histos[i],"res") ) { 
	htest->SetMinimum(0.00);
	href->SetMinimum(0.00);
      }

      //axis labels
      if ( key!="" ) { 
	htest->SetTitle("");
        href->SetTitle("");
	plotname = key+"_";
      }
      else if(contains(chains[j],"EF")){
	htest->SetTitle("");
        href->SetTitle("");
	plotname = "EF_";
      }
      else if(contains(chains[j],"L2")){
	htest->SetTitle("");
	href->SetTitle("");
	plotname = "L2_";
      }
      else if(contains(chains[j],"HLT")){
	htest->SetTitle("");
	href->SetTitle("");
	plotname = "HLT_";
      }
      else if(contains(chains[j],"FTK")){
	htest->SetTitle(("FTK "+histonames[i]).c_str());
	href->SetTitle(("FTK "+histonames[i]).c_str());
	plotname = "FTK_";
      }

      plotname += histos[i]; 

      /// replace the "/" in the filename so we don't try to 
      /// make plots in subdirectories by accident  
      replace(plotname, "/", "_"); 

      //change efficiencies
      if(contains(histos[i],"eff")) {
	if(histos[i]=="pT_eff") { 
	  //	  href->SetMinimum(60);
	  href->GetXaxis()->SetRangeUser(1,100);
	  htest->GetXaxis()->SetRangeUser(1,100);
	  c1->SetLogx();
	  uselogx = true;
	}
	if(histos[i]=="eff_vs_mu") { 
	  //	  href->SetMinimum(60);
	  href->GetXaxis()->SetRangeUser(0,40);
	  htest->GetXaxis()->SetRangeUser(0,40);
	}
	if(histos[i]=="d0_eff" || histos[i]=="a0_eff") { 
	  //	  href->SetMinimum(60);
	  href->GetXaxis()->SetRangeUser(-3,3);
	  htest->GetXaxis()->SetRangeUser(-3,3);
	}
	href->SetMinimum(70);
	href->SetMaximum(105);
	htest->SetMinimum(70);
	htest->SetMaximum(105);
      }

      if(histos[i]=="pT") { 
	href->GetXaxis()->SetRangeUser(1,100);
	htest->GetXaxis()->SetRangeUser(1,100);
      }

      if(contains(histos[i],"rd0_vs_pt" ))  {
        htest->GetXaxis()->SetRangeUser(0,80);
        href->GetXaxis()->SetRangeUser(0,80);
      }

      if(contains(histos[i],"rd0_vs_eta" ))  {
      	href->GetYaxis()->SetMoreLogLabels(true);
 	htest->GetYaxis()->SetMoreLogLabels(true);
	//	href->SetMaximum(2.);
	//     	href->SetMinimum(0.02);
	//    	htest->SetMaximum(2.);
	//    	htest->SetMinimum(0.02);
	//    	href->SetMaximum(0.5);
	//	href->SetMinimum(0);
	//   	htest->SetMaximum(0.5);
	//	htest->SetMinimum(0);
      }


      std::string c = chains[j];

      if ( c.find("_IDTrkNoCut")!=std::string::npos ) c.erase( c.find("_IDTrkNoCut"), 11 );
      if ( c.find("_idperf")!=std::string::npos )     c.erase( c.find("_idperf"), 7 );
      if ( c.find("_bperf")!=std::string::npos )      c.erase( c.find("_bperf"), 6 );
      if ( c.find("xAODCnv")!=std::string::npos )     c.erase( c.find("xAODCnv"), 7 );
      if ( c.find("Tracking")!=std::string::npos ) c.replace( c.find("Tracking"), 8, "Trk" );    

      //      if ( c.find("_EFID")!=std::string::npos )     c.erase( c.find("_EFID"), 5 );

      replace( c, "_Tr", " :  " );
      replace( c, "_In", " :  " );

      c = "  " + c;

      //      plots.push_back( Plotter( htest, href, plotname ) );
      plots.push_back( Plotter( htest, href, c ) );


      if ( make_ref_efficiencies ) { 
	
	if ( htestnum && hrefnum ) { 
	  Efficiency e( htestnum, hrefnum, "" );

	  TH1* h = e.Hist();

	  double range = h->GetMaximum()-h->GetMinimum();

	  if ( range<20 ) { 
	    double _max = int( (h->GetMaximum() + 20)*0.1 )*10.0;
	    double _min = int( (h->GetMinimum() - 10)*0.1 )*10.0;
	    
	    if ( _max>100 ) _max = 102;
	    if ( _min<0 )   _min = 0;
	    
	    h->SetMinimum(_min);
	    h->SetMaximum(_max);
	  }

	  plots_eff.push_back( Plotter( e.Hist(), 0, c ) );
	  
	}     
      }


      
      if(contains(histos[i],"_res"))  {
	
	/// resolutions 

	TF1* d95 = Resplot::FitNull95( htest );
	
	double   mean_95 = d95->GetParameter(1);
	double  dmean_95 = d95->GetParError(1);
	double    rms_95 = d95->GetParameter(2);
	double   drms_95 = d95->GetParError(2);
	
	//	std::cout <<  "\t\t" << histos[i] 
	//		  << "\tmean:     " << mean_95 << " +- " << dmean_95 
	//		  << "\trms:     "  <<  rms_95 << " +- " << drms_95 << std::endl; 

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

	std::cout <<  "\t\t" << histos[i] 
		  << "\tmean:     " << mean_95 << " +- " << dmean_95 << " : pow " << mean_power
		  << "\trms:     "  <<  rms_95 << " +- " << drms_95  << " : pow " << rms_power << std::endl; 

#if 0       	
	if(histos[i]=="ipT_res") {
	  Mean.push_back(label("mean_{95} = %5.3lf #pm %5.3lf GeV^{-1}", mean_95, dmean_95));
	  RMS.push_back(label( "rms_{95}   = %5.3lf #pm %5.3lf GeV^{-1}", rms_95,  drms_95));
	}
	else { 
	  if ( mean_95>=1e-3 &&  rms_95>=1e-3 ) { 
   	    Mean.push_back(label("mean_{95} = %6.4lf #pm %6.4lf ", mean_95, dmean_95));
	    RMS.push_back(label( "rms_{95}   = %6.4lf #pm %6.4lf ", rms_95,  drms_95));
	  }
	  else { 
	    Mean.push_back(label("mean_{95} = ( %5.2lf #pm %5.2lf ) #times 10^{-3}", mean_95*1000, dmean_95*1000));
	    RMS.push_back(label( "rms_{95}   = ( %5.2lf #pm %5.2lf ) #times 10^{-3}", rms_95*1000,  drms_95*1000));
	  }

	}
#endif

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

	
	TF1* d95ref = Resplot::FitNull95( href );
	
	double   mean_95ref = d95ref->GetParameter(1);
        double  dmean_95ref = d95ref->GetParError(1);
        double    rms_95ref = d95ref->GetParameter(2);
        double   drms_95ref = d95ref->GetParError(2);
	
	std::cout <<  "\t\t" << histos[i]
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
					   

#if 0
	if(histos[i]=="ipT_res") {
          MeanRef.push_back(label("mean_{95} ref = %5.3lf #pm %5.3lf GeV^{-1}", mean_95ref, dmean_95ref));
          RMSRef.push_back(label( "rms_{95}  ref = %5.3lf #pm %5.3lf GeV^{-1}", rms_95ref,  drms_95ref));
        }
        else {
	  if ( mean_95ref>=1e-3 &&  rms_95ref>=1e-3 ) { 
	    MeanRef.push_back(label("mean_{95} ref = %6.4lf #pm %6.4lf ", mean_95ref, dmean_95ref));
	    RMSRef.push_back(label( "rms_{95}  ref = %6.4lf #pm %6.4lf ", rms_95ref,  drms_95ref));
	  }
	  else {
	    MeanRef.push_back(label("mean_{95} ref = ( %5.2lf #pm %5.2lf ) #times 10^{-3}", mean_95ref*1000, dmean_95ref*1000));
	    RMSRef.push_back(label( "rms_{95}  ref = ( %5.2lf #pm %5.2lf ) #times 10^{-3}", rms_95ref*1000,  drms_95ref*1000));
	  }
        }
#endif
	
        htest->Sumw2();
        href->Sumw2();
        htest->Scale(1./NeventTest);
        href->Scale(1./NeventRef);
	
      }
      
    }  

    if ( contains(histos[i],"_res")) {
      /// use 20 times the max from any of the plots as the upper limit so there's room for the key 
      /// (as all residual plots are on a log scale)   
      plots.Max(20);  

      if ( contains(histos[i],"ipT")) plots.xrange(false);   // sets the xrange to something sensible (and symmetric)
      else                            plots.xrange(true);  // sets the xrange to something sensible
    }


    if ( contains(histos[i],"_vs_") && !contains(histos[i],"eff") ) { 
      std::cout << "plots: " << histos[i] << std::endl;
      /// use 1.5 times the actual maximum so room for the key
      plots.Max(1.5); 
    }

    //    if ( contains(histos[i],"sigma") ) plots.Min(0.3); 
    if ( contains(histos[i],"sigma") ){ 
      // plots.LogY(true);
      plots.Min(0); 
    }


    plots.Draw( legend );
    /*if ( histos[i],"eff")) {
      ATLASFORAPP_LABEL(0.15,0.2,1,0.05);
      myText(      0.7,0.3,1,"#sqrt{s}= 8 TeV", 0.045);
      myText(      0.7,0.2,1,"#intL dt = 20.3 fb^{-1}", 0.045); 
    }
    else {
      ATLASFORAPP_LABEL(0.6,0.85,1,0.05);
      myText(      0.6,0.8,1,"#sqrt{s}= 8 TeV", 0.045);
      myText(      0.6,0.7,1,"#intL dt = 20.3 fb^{-1}", 0.045); 
      }
    */
    
    if ( uselogx ) c1->SetLogx(true);


    if ( (contains(histos[i],"_res") || histos[i]=="pT" || contains(histos[i],"vs_pt") ) && !contains(histos[i],"sigma") ) { 
      c1->SetLogy(true);

      if ( contains(histos[i],"_res") ){
	for ( unsigned j=0 ; j<chains.size() ; j++ ) { 
	  if ( j<MeanRef.size() ) { 
	    DrawLabel( 0.13, (0.6-j*0.035), MeanRef[j], colours[j%6] );
	    DrawLabel( 0.13, (0.6-0.035*chains.size()-j*0.035)-0.01, RMSRef[j],  colours[j%6] );
	    DrawLabel( 0.62, (0.6-j*0.035), Mean[j],  colours[j%6] );
	    DrawLabel( 0.62, (0.6-0.035*chains.size()-j*0.035)-0.01, RMS[j],  colours[j%6] );
	  }
	}
      }

    }


    if ( plotname!="" ) { 
      plots.back().Print( dir+plotname+tag+".pdf" );
      plots.back().Print( dir+plotname+tag+".png" );
    }    


    if ( make_ref_efficiencies ) { 

      plots_eff.SetXaxisTitle( plots.GetXaxisTitle() ); 
      plots_eff.SetYaxisTitle( plots.GetYaxisTitle() ); 

      plots_eff.Draw( legend_eff );

      if ( contains(plotname,"pT") && contains(plotname,"eff") ) { 
	gPad->SetLogx(true);
      }

      if ( plotname!="" ) { 
	plots_eff.back().Print( dir+plotname+tag+"_refeff.pdf" );
	plots_eff.back().Print( dir+plotname+tag+"_refeff.png" );
	gPad->SetLogx(true);
      }    

      
    }

    //    std::cout << "delete c1 " << c1 << std::endl;
    delete c1;
    //  std::cout << "deleted " << std::endl;
    
  }
  
  
  if ( _ftest ) delete _ftest;
  if ( _fref )  delete _fref;

  return 0;
}
