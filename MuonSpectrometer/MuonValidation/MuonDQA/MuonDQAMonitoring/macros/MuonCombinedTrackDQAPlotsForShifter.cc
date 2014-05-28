/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================//
// *** MuonCombinedTrackDQAPlotsForShifter.cc *************************************************** //
// DESCRIPTION:                                                                                   //
//  ROOT Macro for extracting to a single ps all Offline Muon Monitoring Histos useful to RTT     //
//  Authors:  Nektarios Chr. Benekos (Illinois)                                                   //
//            Dominique Fortin (TRIUMF)                                                           //
//     ...                                                                                        //
// TO EXECUTE, in ROOT command line enter:                                                        //
//  .x MuonCombinedTrackDQAPlotsForShifter.cc("fileName")                                         // 
//================================================================================================//

#include <string>
#include <vector>

void MuonCombinedTrackDQAPlotsForShifter( string fName="" )
{ 
  //gROOT->Macro("atlas_style.cc");
  const char* MBoy  = "ConvertedMBoy";
  const char* Moore = "Moore";
  
  TFile* f=new TFile(fName.c_str(),"READ");

  //Get the run number - it's the top-folder's name in the root file!
  TIter nextcd0(gDirectory->GetListOfKeys());
  TKey *key0;
  key0 = (TKey*)nextcd0();
  TDirectory *dir0= (TDirectory*)key0->ReadObj();
  string runNumber=dir0->GetName(); //runNumber.substr(4) returns only the number
  std::string pdfTag=runNumber+".pdf";
  TDirectory *dir;
  TCanvas *canv=new TCanvas("canv","Spectrometer Hists",0,0,850,650);
  TPad *pad=(TPad*) canv;
  canv->Clear();
  TPDF* pdf;
  stringstream pdfName;
  pdfName<<"MuonCombinedTrackDQAPlotsForShifter_"<<pdfTag;
  pdf=new TPDF(pdfName.str().c_str());

  int npage=0;      
  int ipad=0;
  


  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - authorship <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  int doStats = 0;

  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_author");
  histname[0].push_back("mucbtrk_cutflow");
  histname[0].push_back("mucbtrk_SummaryComb"); 
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2) pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Multiplicity distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = 1;

  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_numpixhits");
  histname[0].push_back("mucbtrk_scthits");
  histname[0].push_back("mucbtrk_numTRThits"); 
  histname[0].push_back("mucbtrk_numMDThits"); 
  histname[0].push_back("mucbtrk_numtrighits"); 
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;


  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Differences between ID and MS <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = 1;

  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dd0");
  histname[0].push_back("mucbtrk_dz0");
  histname[0].push_back("mucbtrk_dPhi0");
  histname[0].push_back("mucbtrk_dTheta0"); 
  histname[0].push_back("mucbtrk_dqp0");
  histname[0].push_back("mucbtrk_dmom0");
  // histname[0].push_back("mucbtrk_qopidvsms");
  // histname[0].push_back("mucbtrk_dq");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;


  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring -Pull distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();


  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = 1;

  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dq");
  histname[0].push_back("mucbtrk_pulld0");
  histname[0].push_back("mucbtrk_pullz0");
  histname[0].push_back("mucbtrk_pullphi0");
  histname[0].push_back("mucbtrk_pulltheta0");
  histname[0].push_back("mucbtrk_pullqp0");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++; 

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Some Control plots <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = 1;


  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_Rec_d0");
  histname[0].push_back("mucbtrk_Rec_z0");
  histname[0].push_back("mucbtrk_Rec_pT");
  histname[0].push_back("mucbtrk_HighpT");
  //  histname[0].push_back("mucbtrk_Rec_qOverP");
  histname[0].push_back("mucbtrk_Rec_theta0");
  histname[0].push_back("mucbtrk_Rec_Phi0");  
  // histname[0].push_back("mucbtrk_Rec_eta");
  histname[0].push_back("mucbtrk_Rec_EtaPhi");
  histname[0].push_back("mucbtrk_Rec_chisqDoF");
  histname[0].push_back("mucbtrk_Rec_charge");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - 2D histos distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = -1;


  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_chisqoDoF_eta");
  histname[0].push_back("mucbtrk_chisqoDoF_phi");
  histname[0].push_back("mucbtrk_Rec_EtaPhi");
  histname[0].push_back("mucbtrk_ElossTrkVsElossCalo");
  // histname[0].push_back("mucbtrk_Rec_pT_vrs_eta");
  // histname[0].push_back("mucbtrk_Rec_pT_vrs_phi");
  // histname[0].push_back("mucbtrk_chisqoDoF_pT");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;

 
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring -  <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();


  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = -1;


  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dpTvsd0");
  histname[0].push_back("mucbtrk_dpTvsz0");
  histname[0].push_back("mucbtrk_dpTvspT");
  histname[0].push_back("mucbtrk_dpTvseta");
  histname[0].push_back("mucbtrk_dpTvsphi");
   
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Eloss distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = -1;

  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_ElossCalo_eta");
  histname[0].push_back("mucbtrk_ElossCalo_phi");
  histname[0].push_back("mucbtrk_ElossMSID_eta");
  histname[0].push_back("mucbtrk_ElossMSID_phi");
  //  histname[0].push_back("mucbtrk_ElossTrkVsElossCalo");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;
     
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Eloss distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  doStats = 1;

  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_deltaEloss_BA");
  // histname[0].push_back("mucbtrk_deltaEloss_BC");
  histname[0].push_back("mucbtrk_deltaEloss_EA");
  // histname[0].push_back("mucbtrk_deltaEloss_EC");
  histname[0].push_back("mucbtrk_deltaElossX_BA");
  // histname[0].push_back("mucbtrk_deltaElossX_BC");
  histname[0].push_back("mucbtrk_deltaElossX_EA");
  // histname[0].push_back("mucbtrk_deltaElossX_EC");
  histname[0].push_back("mucbtrk_deltaElossVsEta");
  histname[0].push_back("mucbtrk_deltaElossVsPhi");
  
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection",doStats);
      canv->Update();
    }
  }
  npage++;
  pdf->Close();
  return;   
}
//=====================================================================================================
// doStats   < 0 = false;  0 = use first bin for stats;  > 0 = use full histo for stats

void drawSupImp(string dirname, string histname,TFile *f, string TrkOrSegm, int doStats) {   
 
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string dirname1=dirname+"StacoMuon"+TrkOrSegm;
  string dirname2=dirname+"MuidMuon"+TrkOrSegm;;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);

  hist1->SetFillColor(0);
  hist2->SetFillColor(0);


  TLegend *leg = new TLegend(0.58,0.9,1,1); 


  float norm1 = hist1->GetEntries();
  float norm2 = hist2->GetEntries();
  float mean1 = 0.;
  float mean2 = 0.;
  float rms1  = 0.;
  float rms2  = 0.;

  // Special case:  Use first bin to normalize (e.g. for muon author)
  if ( doStats == 0 ) {   
    norm1 = hist1->GetBinContent(1);
    norm2 = hist2->GetBinContent(1);
  }

  if ( doStats > -1 ) {
    if ( !hist1->IsA()->InheritsFrom("TH2") && !hist1->IsA()->InheritsFrom("TProfile")) {
      if (norm1 > 0. ) hist1->Scale( 1./ norm1 );
      if (norm2 > 0. ) hist2->Scale( 1./ norm2 );
      mean1 = hist1->GetMean();
      mean2 = hist2->GetMean();
      rms1  = hist1->GetRMS();
      rms2  = hist1->GetRMS();
    }
  }
  double max =hist1->GetMaximum();
  double min =hist1->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();

  bool plotFirst = true;
  if ( max < max2 ){ 
    max = max2;
    plotFirst = false;
  }
  if ( min > min2 ){ min = min2;}
  if (min > 0) min = 0.;

  if ( !hist1->IsA()->InheritsFrom("TH2") && !hist1->IsA()->InheritsFrom("TProfile")) { 
    //    hist2->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
    if ( plotFirst ) {
      if ( doStats != 0) hist1->GetYaxis()->SetRangeUser(1.2*min,1.2*max);
      hist1->Draw();
      hist2->Draw("same");
    } else {
      if ( doStats != 0) hist2->GetYaxis()->SetRangeUser(1.2*min,1.2*max);
      hist2->Draw();
      hist1->Draw("same");
    }
  } 
  else if ( hist1->IsA()->InheritsFrom("TH2") ) {
    hist1->Draw("BOX");
    hist2->Draw("sameBOX");
  }
  else {
    hist1->Draw();
    hist2->Draw("same");
  }

  char t1[100];
  char t2[100];
  if ( doStats > 0 && (!hist1->IsA()->InheritsFrom("TH2") || !hist1->IsA()->InheritsFrom("TProfile")) ){
    sprintf(t1,"Staco entries= %d;  mean= %6.3f;  rms= %6.3f",hist1->GetEntries(),hist1->GetMean(1),hist1->GetRMS(1));
    sprintf(t2,"Muid  entries= %d;  mean= %6.3f;  rms= %6.3f",hist2->GetEntries(),hist2->GetMean(1),hist2->GetRMS(1));
  }    
  else if ( doStats == 0 && (!hist1->IsA()->InheritsFrom("TH2") || !hist1->IsA()->InheritsFrom("TProfile")) ){
    sprintf(t1,"Staco: number of muons= %d",norm1);
    sprintf(t2,"Muid:  number of muons= %d",norm2);

  } 
  else {
    sprintf(t1,"Staco entries: %d",hist1->GetEntries());
    sprintf(t2,"Muid  entries: %d",hist2->GetEntries());
  }


  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  
  cout<<"---"<<histname<<endl;
  return;
}

 
