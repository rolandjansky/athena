/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================//
// *** MuonCombinedDQAPlotsForRTT.cc ****************************************************************     //
// DESCRIPTION:                                                                                   //
//  ROOT Macro for extracting to a single ps all Offline Muon Monitoring Histos useful to RTT     //
//  Authors:  Nektarios Chr. Benekos (Illinois) Ioannis Nomidis (AUTh - Thessaloniki, March 2009) //
//     ...                                                                                        //
// TO EXECUTE, in ROOT command line enter:                                                        //
//  .x MuonCombinedDQAPlotsForRTT.cc("fileName")                                                          // 
//================================================================================================//

#include <string>
#include <vector>

void MuonCombinedDQAPlotsForRTT( string fName="" )
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
  std::string psTag=runNumber+".ps";
  TDirectory *dir;
  TCanvas *canv=new TCanvas("canv","Spectrometer Hists",0,0,850,650);
  TPad *pad=(TPad*) canv;
  canv->Clear();
  TPostScript* ps;
  stringstream psName;
  psName<<"MuonCombinedDQAPlotsForRTT_"<<psTag;
  ps=new TPostScript(psName.str().c_str());
  int npage=0;      
  int ipad=0;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing CombinedMuonMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/MuonCombined/Tracks/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("author");
  histname[0].push_back("nTracks");  
  histname[0].push_back("charge");
  histname[0].push_back("charge_InDet_vs_Muon");
  histname[0].push_back("chi2");
  histname[0].push_back("match_chiSq");
  histname[0].push_back("ndof");
  histname[0].push_back("prob"); 
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"_all");
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing CombinedMuonMonitoring Track Parameters <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/MuonCombined/Tracks/"); //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("d0");
  histname[0].push_back("z0");
  histname[0].push_back("eta");
  histname[0].push_back("phi");
  histname[0].push_back("momentum");
  histname[0].push_back("pt");
  histname[0].push_back("ptLow");
  histname[0].push_back("ptHigh");
  histname[0].push_back("p_InDet_vs_Muon");
  histname[0].push_back("pT_InDet_vs_Muon");
 
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"_all");
      canv->Update();
    }
  }
  npage++;

}
//=====================================================================================================

void drawSupImp(string dirname, string histname,TFile *f, string TrkOrSegm)
{   
 
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string dirname1=dirname+"staco"+TrkOrSegm;
  string dirname2=dirname+"muid"+TrkOrSegm;;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  TLegend *leg = new TLegend(0.58,0.9,1,1); 
  double max=hist1->GetMaximum();
  double min=hist2->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();
  if ( max< max2 )  max=max2;
  if ( min< min2 )  min=min2;
  if (min>0) min=0;
  if (!hist1->IsA()->InheritsFrom("TH2")) { 
    hist1->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
  }
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);
  hist1->Draw();
  hist2->Draw("same");
  char t1[100];
  char t2[100];
  sprintf(t1,"StacoTracks entries: %d",hist1->GetEntries());
  sprintf(t2,"MuidTracks entries: %d",hist2->GetEntries());
  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  cout<<"---"<<histname<<endl;
  return;
}

 
