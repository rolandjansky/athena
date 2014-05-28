/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================//
// *** MuonCombinedTrackDQAPlotsForRTT.cc ***************************************************     //
// DESCRIPTION:                                                                                   //
//  ROOT Macro for extracting to a single ps all Offline Muon Monitoring Histos useful to RTT     //
//  Authors:  Nektarios Chr. Benekos (Illinois)                                                   //
//     ...                                                                                        //
// TO EXECUTE, in ROOT command line enter:                                                        //
//  .x MuonCombinedTrackDQAPlotsForRTT.cc("fileName")                                             // 
//================================================================================================//

#include <string>
#include <vector>

void MuonCombinedTrackDQAPlotsForRTT( string fName="" )
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
  psName<<"MuonCombinedTrackDQAPlotsForRTT_"<<psTag;
  ps=new TPostScript(psName.str().c_str());
  int npage=0;      
  int ipad=0;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - CutFlow distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_cutflow");
  histname[0].push_back("mucbtrk_numpixhits");
  histname[0].push_back("mucbtrk_scthits");
  histname[0].push_back("mucbtrk_numTRThits"); 
  histname[0].push_back("mucbtrk_numMDThits"); 
  histname[0].push_back("mucbtrk_numtrighits"); 
  histname[0].push_back("mucbtrk_SummaryComb"); 
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
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
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_pulld0");
  histname[0].push_back("mucbtrk_pulld0_pos");
  histname[0].push_back("mucbtrk_pulld0_neg");
  histname[0].push_back("mucbtrk_pullz0");
  histname[0].push_back("mucbtrk_pullz0_pos");
  histname[0].push_back("mucbtrk_pullz0_neg"); 
  histname[0].push_back("mucbtrk_pullphi0");
  histname[0].push_back("mucbtrk_pullphi0_pos");
  histname[0].push_back("mucbtrk_pullphi0_neg");  
  histname[0].push_back("mucbtrk_pulltheta0");
  histname[0].push_back("mucbtrk_pulltheta0_pos");
  histname[0].push_back("mucbtrk_pulltheta0_neg");
  histname[0].push_back("mucbtrk_pullqp0");
  histname[0].push_back("mucbtrk_pullqp0_pos");
  histname[0].push_back("mucbtrk_pullqp0_neg");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;


  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Pull Barrel distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_pullRphi_Barrel");
  histname[0].push_back("mucbtrk_pullz_Barrel");
  histname[0].push_back("mucbtrk_pullPhi_Barrel");
  histname[0].push_back("mucbtrk_pulltheta_Barrel");
  histname[0].push_back("mucbtrk_pullqp_Barrel");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - Pull EndCap distributions <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_pullR_Endcap");
  histname[0].push_back("mucbtrk_pullPhi_Endcap");
  histname[0].push_back("mucbtrk_pullphi_Endcap");
  histname[0].push_back("mucbtrk_pulltheta_Endcap");
  histname[0].push_back("mucbtrk_pullqp_Endcap");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
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
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_Rec_EtaPhi");
  histname[0].push_back("mucbtrk_Rec_pT_vrs_eta");
  histname[0].push_back("mucbtrk_Rec_pT_vrs_phi");
  histname[0].push_back("mucbtrk_chisqoDoF_eta");
  histname[0].push_back("mucbtrk_chisqoDoF_pT");
  histname[0].push_back("mucbtrk_chisqoDoF_mdtHits");
  histname[0].push_back("mucbtrk_chisqoDoF_TrigHits");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
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
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dpvsd0");
  histname[0].push_back("mucbtrk_dpTvsd0");
  histname[0].push_back("mucbtrk_dpvsz0");
  histname[0].push_back("mucbtrk_dpTvsz0"); 
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - dpvsp and dpTvdpT <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dpTvspTlow_Down");
  histname[0].push_back("mucbtrk_dpTvspTlow_Up");
  histname[0].push_back("mucbtrk_dpTvspThigh_Down");
  histname[0].push_back("mucbtrk_dpTvspThigh_Up");
  histname[0].push_back("mucbtrk_dpvsplow_Down");
  histname[0].push_back("mucbtrk_dpvsplow_Up");
  histname[0].push_back("mucbtrk_dpvsphigh_Down");
  histname[0].push_back("mucbtrk_dpvsphigh_Up");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - pT <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_1overpT");
  histname[0].push_back("mucbtrk_pT");
  histname[0].push_back("mucbtrk_PospT");
  histname[0].push_back("mucbtrk_NegpT");
  histname[0].push_back("mucbtrk_HighpT");
  histname[0].push_back("mucbtrk_PosHighpT");
  histname[0].push_back("mucbtrk_NegHighpT");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - pT<0 in BA,BC,EA,EC <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_NegpT_BA");
  histname[0].push_back("mucbtrk_NegpT_BC");
  histname[0].push_back("mucbtrk_NegpT_EA");
  histname[0].push_back("mucbtrk_NegpT_EC");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - pt>0 in BA,BC,EA,EC<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_PospT_BA");
  histname[0].push_back("mucbtrk_PospT_BC");
  histname[0].push_back("mucbtrk_PospT_EA");
  histname[0].push_back("mucbtrk_PospT_EC");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - pT<0 in upper and lower detector<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_NegpT_Up");
  histname[0].push_back("mucbtrk_NegpT_Down");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - dq<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dq");
  histname[0].push_back("mucbtrk_dq_Barrel");
  histname[0].push_back("mucbtrk_dq_Endcap");
  histname[0].push_back("mucbtrk_dqp0");
  histname[0].push_back("mucbtrk_dqp0_pos");
  histname[0].push_back("mucbtrk_dqp0_neg");
  histname[0].push_back("mucbtrk_dqp_Barrel");
  histname[0].push_back("mucbtrk_dqp_Endcap");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  
  
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - dq<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_dz0");
  histname[0].push_back("mucbtrk_dz0_neg");
  histname[0].push_back("mucbtrk_dz)_pos");
  histname[0].push_back("mucbtrk_Barrel");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing MuonCombinedTrackMonitoring - pT>0 in upper and lower detector<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  vector<string> dirname_v;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonCombinedTrackMonitoring/"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_PospT_Up");
  histname[0].push_back("mucbtrk_PospT_Down");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
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
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_deltaEloss_BA");
  histname[0].push_back("mucbtrk_deltaEloss_BC");
  histname[0].push_back("mucbtrk_deltaEloss_EA");
  histname[0].push_back("mucbtrk_deltaEloss_EC");
  histname[0].push_back("mucbtrk_deltaElossX_BA");
  histname[0].push_back("mucbtrk_deltaElossX_BC");
  histname[0].push_back("mucbtrk_deltaElossX_EA");
  histname[0].push_back("mucbtrk_deltaElossX_EC");
  histname[0].push_back("mucbtrk_deltaElossVsEta");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
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
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("mucbtrk_ElossCalo_BA");
  histname[0].push_back("mucbtrk_ElossCalo_BC");
  histname[0].push_back("mucbtrk_ElossCalo_EA");
  histname[0].push_back("mucbtrk_ElossCalo_EC");
  histname[0].push_back("mucbtrk_ElossMSID_BA");
  histname[0].push_back("mucbtrk_ElossMSID_BC");
  histname[0].push_back("mucbtrk_ElossMSID_EA");
  histname[0].push_back("mucbtrk_ElossMSID_EC");

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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
      canv->Update();
    }
  }
  npage++;
  

 
  
  // 
  // 
  // 
  //   cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing CombinedMuonMonitoring Track Parameters <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  //   canv->Clear();  ipad=0;
  //   dirname_v.clear();
  //   dirname_v.push_back(runNumber+"/MuonCombined/Tracks/"); //0
  //   histname.clear();
  //   for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  //   histname[0].push_back("d0");
  //   histname[0].push_back("z0");
  //   histname[0].push_back("eta");
  //   histname[0].push_back("phi");
  //   histname[0].push_back("momentum");
  //   histname[0].push_back("pt");
  //   histname[0].push_back("ptLow");
  //   histname[0].push_back("ptHigh");
  //   histname[0].push_back("p_InDet_vs_Muon");
  //   histname[0].push_back("pT_InDet_vs_Muon");
  //  
  //   int nhist=0;
  //   for (int i=0; i<histname.size(); i++)
  //     for (int j=0; j<histname[i].size(); j++) {
  //       nhist++;
  //       //cout<<dirname_v[i]<<histname[i][j]<<endl;
  //     }
  //   if (nhist<2)  pad->cd(0);
  //   else if (nhist<3)  canv->Divide(1,2);
  //   else if (nhist<5)  canv->Divide(2,2);
  //   else if (nhist<7)  canv->Divide(2,3);
  //   else if (nhist<9)  canv->Divide(2,4);
  //   else if (nhist<10) canv->Divide(3,3);
  //   else if (nhist<13) canv->Divide(3,4);
  //   else if (nhist<17) canv->Divide(4,4);
  //   else if (nhist<30) canv->Divide(5,5);
  // 
  //   for (int idir=0; idir<dirname_v.size(); idir++) {
  //     for (int ihist=0; ihist<histname[idir].size(); ihist++) {
  //       canv->cd(++ipad);
  //       drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Collection");
  //       canv->Update();
  //     }
  //   }
  //   npage++;
  ps->Close();
  return;   
}
//=====================================================================================================

void drawSupImp(string dirname, string histname,TFile *f, string TrkOrSegm)
{   
 
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string dirname1=dirname+"StacoMuon"+TrkOrSegm;
  string dirname2=dirname+"MuidMuon"+TrkOrSegm;;
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

 
