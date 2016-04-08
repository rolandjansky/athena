/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <CompHist.h>
#include <HisToKomp.C>
#include <CompHistFileConfig.C>

HisToKomp *DataHis;

char FileName[80];
char CanvasName[80];
char GeneralName[40]="figures";
char Line[30];
int  HistosLevel=0;
char ImageType[3]="png";

int FConfig;

//////////////////////////////////////////////////
void CompHist(int type=0, int level=2){
  
  cout << " +---------------------+ " << endl;
  cout << " | Printing histograms | " << endl;
  cout << " +---------------------+ " << endl;
  cout << " | Track selection: " << level << "  | " << endl;
  cout << " +---------------------+ " << endl;

  FConfig = type;
  HistosLevel= level;
  DataHis = new HisToKomp("Data histograms");
  DataHis->SetDirectory("trkana");
  load_file_configuration(FConfig);
  load_CanvasTypes();

  //Prepare a directory to store the plots
  sprintf(Line,".!mkdir -p %s",GeneralName);
  gROOT->ProcessLine(Line);

  //gROOT->ProcessLine(".L ../../monitor/include/utils.cc");
  

  if (1==0) {
    draw_tops_carlos();
    return;
  }
  //draw_histos_SCTBarrelTime();
   
  //return;
  // trkana histograms
  Print_StatsTable("Track_Parameters/hRecoPt");
  if (1==0) {
    draw_histos_xx();
    // Ben IEEE    
    //draw_histos_hits();
    //draw_histos_ResSil();
    //draw_histos_TrkPropClaude();
    //draw_histos_PullPIXBarrel();
    //draw_histos_ResPIXBarrelWide();
    //draw_histos_ResPIXBarrelClaude();
    //draw_histos_ResSCTBarrelWide(); 
    //draw_histos_ResPIXBarrelPhi();
    //draw_histos_ResSCTEndcaps();
    return;
  }
  draw_histos_TrkProp(); 
  draw_histos_ntracks();
  draw_histos_ResPIXBarrelWide();
  draw_histos_ResPIXEndcapsWide();
  draw_histos_ResSCTBarrelWide();


  draw_histos_ResSCTEndcapsWide();
  draw_histos_PtPhi0();
  draw_histos_Pt_Phi0Eta();
  draw_histos_ResSCTBarrel();
  draw_histos_SCTBarrelEntries();
  draw_histos_SCTBarrelResMean();
  draw_histos_SCTBarrelResMeanAndSigma();
  draw_histos_ResPIXBarrel();
  draw_histos_PIXBarrelEntries();
  draw_histos_PIXBarrelResMean();
  draw_histos_PIXBarrelResMeanAndSigma();
  draw_histos_PIXHitError();


  draw_histos_hits();
  draw_histos_ResSil();
  draw_histos_SCTHitError();

  draw_histos_PIXBarrelHitMap();
  draw_histos_PIXBarrelResMap();
  draw_histos_SCTBarrelResMap();
  draw_histos_HitsPerLayer();
  draw_histos_SystPt();
  draw_histos_ResVsRecoPtSCTBarrel();
  draw_histos_QoverPt();
  draw_histos_Pt();
  draw_histos_Chi2();
  draw_histos_Chi2Evolution();

  draw_histos_PullPIXBarrel();
  draw_histos_PullSCTBarrel();

  draw_histos_TrkMatching();
  draw_histos_TrkMatching_Profiles_Vs_D0();
  draw_histos_TrkMatching_Profiles_Vs_Z0();
  draw_histos_TrkMatching_Profiles_Vs_Phi0();
  draw_histos_TrkMatching_Profiles_Vs_Eta();
  draw_histos_TrkMatching_Profiles_Vs_QoverPt();

  return;

  
  draw_histos_ResSCTBarrelWide();
  //  
  draw_histos_RecoTruth();
  draw_histos_TrkTruth();

  // SCT residuals
  draw_histos_ResSil();
  draw_histos_ResSCTBarrel();
  draw_histos_ResSCTEndcaps();
  draw_histos_ResSCTEndcapsWide();
  draw_histos_PullSCTBarrel();
  draw_histos_PullSCTEndcaps();

  // PIX resiudals
  draw_histos_ResPIXBarrel();
  draw_histos_PIXBarrelEntries();
  draw_histos_PIXBarrelResMean();
  draw_histos_PIXBarrelResMeanAndSigma();
  draw_histos_ResPIXBarrelWide();
  draw_histos_ResPIXEndcaps();
  draw_histos_ResPIXEndcapsWide();
  draw_histos_PullPIXBarrel();
  draw_histos_PullPIXEndcaps();
  draw_histos_PIXBarrelResByRing();
  
  //
  draw_histos_ResVsRecoPtSCTBarrel();

  // draw_histos_SystTheta();
  
  draw_histos_QoverPt();
  draw_histos_D0();
  draw_histos_PtPhi0();
  draw_histos_Pt_Phi0Eta();
  draw_histos_Pt();
  // draw_histos_custom();
  // draw_histos_mass();
  return;
} 

//////////////////////////////////////////////////

void load_file_configuration(int FileConfig) {
  
  cout << "-------------------------------------" << endl;
  cout << "-- CompHist.C -----------------------" << endl;
  cout << "------- loading file config ---- " << FileConfig << " --" << endl;
  cout << "-------------------------------------" << endl;
  
  switch (FileConfig) {
  case 0:
    define_file_config0();
    break;
  case 1:
    define_file_config1();
    break;
  case 2:
    define_file_config2();
    break;
  case 3:
    define_file_config3();
    break;
  case 4:
    define_file_config4();
    break;
  case 5:
    define_file_config5();
    break;
  case 6:
    define_file_config6();
    break;
  case 7:
    define_file_config7();
    break;
  case 8:
    define_file_config8();
    break;
  case 9:
    define_file_config9();
    break;
  case 10:
    define_file_config10();
    break;
  case 11:
    define_file_config11();
    break;
  case 12:
    define_file_config12();
    break;
  case 13:
    define_file_config13();
    break;
  case 13:
    define_file_config14();
    break;
  case 13:
    define_file_config15();
    break;
  default:
    cout << " ** warning ** Uknwon file configuration " << FileConfig 
	 << " loading default configuration" 
	 << endl;
    define_file_config0();
  } 
}

///////////////////////////////////////////////////////////////
// PLOTTING functions
///////////////////////////////////////////////////////////////
void draw_histos_xx() {
  TCanvas *CanXX = new TCanvas("CTMB","CTMB",500,80,800,400);
  CanXX->Divide(2,1);

  CanXX->cd(1);
  DrawMe("Track_Parameters/hRecoPt");
  gPad->SetGridx(1);

  CanXX->cd(2);
  DataHis->Draw("hSTATS");
  gPad->SetGridx(1);
 
  print_canvas(CanXX,"xx");
}

///////////////////////////////////////////////////////////////
void draw_histos_TrkProp() {
  TCanvas *CTrk = new TCanvas("CTrk","TRK reco. properties",100,100,Can22.xSize,Can22.ySize);
  CTrk->Divide(3,2);
  
  CTrk->cd(1);
  DrawMe("Track_Parameters/hRecoPt");
  gPad->SetGridx(1);

  CTrk->cd(2);
  DrawMe("Track_Parameters/hRecoEta");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  CTrk->cd(3);
  DrawMe("Track_Parameters/hRecoPhi0");
  gPad->SetGridx(1);
  
  CTrk->cd(4);
  //DrawMe("Track_Parameters/hRecoD0Wide");
  DrawMe("Track_Parameters/hRecoD0");
  gPad->SetGridx(1);
  
  CTrk->cd(5);
  DrawMe("Track_Parameters/hRecoZ0");
  gPad->SetGridx(1);
  
  CTrk->cd(6);
  // DrawMe("Track_Parameters/hRecoTheta");
  // DrawMe("Track_Parameters/hRecoChi2","seriesplot");
  DrawMe("Track_Parameters/hRecoChi2");
  gPad->SetGridx(1);
    
  print_canvas(CTrk,"TRKRecoProp");
}
///////////////////////////////////////////////////////////////
void draw_histos_TrkPropClaude() {
  TCanvas *CTrkC = new TCanvas("CTrk Claude","TRK reco. properties(Claude)",100,100,Can22.xSize/2,Can22.ySize);
  CTrkC->Divide(1,2);
  
  CTrkC->cd(1);
  DrawMe("Track_Parameters/hRecoPt");
  gPad->SetGridx(1);

  CTrkC->cd(2);
  // DrawMe("Track_Parameters/hRecoTheta");
  // DrawMe("Track_Parameters/hRecoChi2","seriesplot");
  DrawMe("Track_Parameters/hRecoChi2");
  gPad->SetGridx(1);
    
  print_canvas(CTrkC,"TRKRecoPropC");
}


///////////////////////////////////////////////////////////////
void draw_histos_RecoTruth() {
  TCanvas *CanRecoTruth = new TCanvas("CRecoTruth","CRecoTruth",100,100,Can22.xSize,Can22.ySize);
  CanRecoTruth->Divide(3,2);
  
  CanRecoTruth->cd(1);
  DrawMe("Track_Parameters/hRecoPt_TruzPt","printentries");
  gPad->SetGridx(1);

  CanRecoTruth->cd(2);
  DrawMe("Track_Parameters/hRecoEta_TruzEta");
  gPad->SetGridx(1);

  CanRecoTruth->cd(3);
  DrawMe("Track_Parameters/hRecoPhi0_TruzPhi0");
  gPad->SetGridx(1);

  CanRecoTruth->cd(4);
  DrawMe("Track_Parameters/hRecoD0_TruzD0");
  gPad->SetGridx(1);

  CanRecoTruth->cd(5);
  DrawMe("Track_Parameters/hRecoZ0_TruzZ0");
  gPad->SetGridx(1);

  CanRecoTruth->cd(6);
  DrawMe("Track_Parameters/hRecoTheta_TruzTheta");
  gPad->SetGridx(1);

  print_canvas(CanRecoTruth,"Trk_RecoTruth");
}

///////////////////////////////

void draw_histos_TrkTruth() {
  TCanvas *CanTruth = new TCanvas("CTruth","CTruth",100,100,Can22.xSize,Can22.ySize);
  CanTruth->Divide(3,2);
  
  CanTruth->cd(1);
  DrawMe("Track_Parameters/hTruzPt");
  gPad->SetGridx(1);

  CanTruth->cd(2);
  DrawMe("Track_Parameters/hTruzEta");
  
  CanTruth->cd(3);
  DrawMe("Track_Parameters/hTruzPhi0");

  CanTruth->cd(4);
  DrawMe("Track_Parameters/hTruzD0");  
  
  CanTruth->cd(5);
  DrawMe("Track_Parameters/hTruzZ0");
  
  CanTruth->cd(6);
  DrawMe("Track_Parameters/hTruzTheta");
  
  print_canvas(CanTruth,"TrkTruth");
}

////////////////////////////////

void draw_histos_hits() {
  //
  TCanvas *CanHits = new TCanvas("HITs (PIX & SCT)","HITs (PIX & SCT)",75,75,900,450);
  CanHits->Divide(2,1);
  
  CanHits->cd(1);
  DrawMe("PIXHits/hPIXhits","printaverage");
  CanHits->cd(2);
  DrawMe("SCTHits/hSCThits","printaverage");

  strcpy(FileName,"Hits");
  sprintf(CanvasName,"%s/%s__%s.%s",GeneralName,GeneralName,FileName,ImageType);
  print_canvas(CanHits,"Hits");
  
}

////////////////////////////////

void draw_histos_ntracks() {
  
  TCanvas *CanNTracks = new TCanvas("Tracks number","TRK  Number of reco. tracks",100,100,Can22.xSize,Can22.ySize);
  CanNTracks->Divide(2,2);
  
  CanNTracks->cd(1);
  DrawMe("HitsTracks/hNTrk","printaverage");

  CanNTracks->cd(2);
  DrawMe("HitsTracks/hNTrkWide","seriesplot");

  CanNTracks->cd(3);
  DrawMe("PIXHits/hPIXBarrelHits","nolegend");
  CanNTracks->cd(4);
  DrawMe("SCTHits/hSCTBarrelHits","nolegend");

  print_canvas(CanNTracks,"NTracks");
}

////////////////////////////////

void draw_histos_ResSil() {
  TCanvas *CanResSilW = new TCanvas("SIL Res wide","SIL (PIX+SCT) Residuals Wide",10,25,1350,425);
  CanResSilW->Divide(3,1);
  
  CanResSilW->cd(1);
  DrawMe("PIXRes/hPIXPhiResBa");
  gPad->SetGridx(1);

  CanResSilW->cd(2);
  //DrawMe("PIXRes/hPIXEtaResBaWide","printentries");
  DrawMe("PIXRes/hPIXEtaResBaWide");
  gPad->SetGridx(1);

  CanResSilW->cd(3);
  //DrawMe("SCTRes/hSCTResBaWide","printentries");
  DrawMe("SCTRes/hSCTResBaWide");
  gPad->SetGridx(1);

  print_canvas(CanResSilW,"SILBarrelResWide");

  TCanvas *CanResSil = new TCanvas("SIL Res","SIL (PIX+SCT) Residuals",50,50,1225,400);
  CanResSil->Divide(3,1);
  

  CanResSil->cd(1);
  DrawMe("PIXRes/hPIXPhiResBaWide");
  CanResSil->cd(2);
  DrawMe("PIXRes/hPIXEtaResBa");
  CanResSil->cd(3);
  DrawMe("SCTRes/hSCTResBa");

  print_canvas(CanResSil,"SILBarrelRes");
}

////////////////////////////////
void draw_histos_ResSCTBarrel() {
  // 
  TCanvas *CanSCTRes = new TCanvas("CResSCTBarrel","SCT Barrel Residuals",100,100,Can22.xSize,Can22.ySize);
  CanSCTRes->Divide(2,2);
  
  CanSCTRes->cd(1);
  DrawMe("SCTRes/hSCTResBaLay0","printeentries,printaverage,PrintMeanAndRMS");
  gPad->SetGridx(1);
  
  CanSCTRes->cd(2);
  DrawMe("SCTRes/hSCTResBaLay1","printeentries");
  gPad->SetGridx(1);

  CanSCTRes->cd(3);
  DrawMe("SCTRes/hSCTResBaLay2","printeentries");
  gPad->SetGridx(1);

  CanSCTRes->cd(4);
  DrawMe("SCTRes/hSCTResBaLay3","printeentries");
  gPad->SetGridx(1);

  print_canvas(CanSCTRes,"SCTBarrelRes");
}

////////////////////////////////
void draw_histos_ResSCTBarrelWide() {
  // 
  TCanvas *CanSCTResWide = new TCanvas("SCTResWide","SCT Barrel Residuals Wide",100,100,Can22.xSize,Can22.ySize);
  CanSCTResWide->Divide(2,2);
  
  CanSCTResWide->cd(1);
  DrawMe("SCTRes/hSCTResBaLay0Wide");
  //DrawMe("SCTRes/hSCTResBaLay0Wide","PrintAverage");
  //DrawMe("SCTRes/hSCTResBaLay0Wide","PrintAverage,PrintEntries,PrintMeanAndRMS");
  //  DrawMe("SCTRes/hSCTResBaLay0Wide","PrintEntries");
  gPad->SetGridx(1);

  CanSCTResWide->cd(2);
  DrawMe("SCTRes/hSCTResBaLay1Wide");
  //DrawMe("SCTRes/hSCTResBaLay1Wide","PrintEntries");
  // DrawMe("SCTRes/hSCTResBaLay1Wide","PrintAverage");
  // DrawMe("SCTRes/hSCTResBaLay1Wide","PrintAverage,PrintEntries");
  gPad->SetGridx(1);

  CanSCTResWide->cd(3);
  DrawMe("SCTRes/hSCTResBaLay2Wide");
  //DrawMe("SCTRes/hSCTResBaLay2Wide","PrintEntries");
  // DrawMe("SCTRes/hSCTResBaLay2Wide","PrintAverage");
  // DrawMe("SCTRes/hSCTResBaLay2Wide","PrintAverage,PrintEntries");
  gPad->SetGridx(1);

  CanSCTResWide->cd(4);
  DrawMe("SCTRes/hSCTResBaLay3Wide");
  //DrawMe("SCTRes/hSCTResBaLay3Wide","PrintEntries");
  // DrawMe("SCTRes/hSCTResBaLay3Wide","PrintAverage");
  // DrawMe("SCTRes/hSCTResBaLay3Wide","PrintAverage,PrintEntries");
  gPad->SetGridx(1);

  print_canvas(CanSCTResWide,"SCTBarrelResWide");
}

////////////////////////////////
void draw_histos_SCTBarrelEntries() {
  // 
  TCanvas *CanSCTEnt = new TCanvas("SCTBarrelEntries","SCT Barrel Entries",100,100,Can22.xSize,Can22.ySize);
  CanSCTEnt->Divide(2,2);
  
  CanSCTEnt->cd(1);
  DrawMe("SCTRes/hSCTResBaLay0","seriesplot,entries");
  gPad->SetGridx(1);
  
  CanSCTEnt->cd(2);
  DrawMe("SCTRes/hSCTResBaLay1","seriesplot,entries");
  gPad->SetGridx(1);

  CanSCTEnt->cd(3);
  DrawMe("SCTRes/hSCTResBaLay2","seriesplot,entries");
  gPad->SetGridx(1);

  CanSCTEnt->cd(4);
  DrawMe("SCTRes/hSCTResBaLay3","seriesplot,entries");
  gPad->SetGridx(1);

  print_canvas(CanSCTEnt,"SCTBarrelEntries");
}

////////////////////////////////
void draw_histos_SCTBarrelResMean() {
  // 
  TCanvas *CanSCTMean = new TCanvas("SCTBarrelResMean","SCT Barrel Residual mean",100,100,Can22.xSize,Can22.ySize);
  CanSCTMean->Divide(2,2);
  
  CanSCTMean->cd(1);
  DrawMe("SCTRes/hSCTResBaLay0","seriesplot");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  CanSCTMean->cd(2);
  DrawMe("SCTRes/hSCTResBaLay1","seriesplot");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanSCTMean->cd(3);
  DrawMe("SCTRes/hSCTResBaLay2","seriesplot");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanSCTMean->cd(4);
  DrawMe("SCTRes/hSCTResBaLay3","seriesplot");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(CanSCTMean,"SCTBarrelResMean");
}

///////////////////////////////////////////////////////////
void draw_histos_SCTBarrelResMeanAndSigma() {
  // 
  TCanvas *CanSCTMeanSig = new TCanvas("SCTBarrelResMeanSigma","SCT Barrel Residual mean and sigma",100,100,Can22.xSize,Can22.ySize);
  CanSCTMeanSig->Divide(2,2);
  
  CanSCTMeanSig->cd(1);
  DrawMe("SCTRes/hSCTResBaLay0","seriesplot,MeanAndSigma");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  CanSCTMeanSig->cd(2);
  DrawMe("SCTRes/hSCTResBaLay1","seriesplot,MeanAndSigma");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanSCTMeanSig->cd(3);
  DrawMe("SCTRes/hSCTResBaLay2","seriesplot,MeanAndSigma");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanSCTMeanSig->cd(4);
  DrawMe("SCTRes/hSCTResBaLay3","seriesplot,MeanAndSigma");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(CanSCTMeanSig,"SCTBarrelResMeanAndSigma");
}


////////////////////////////////
void draw_histos_SCTBarrelHitMap() {
  // 

    
  TCanvas *CanSCTResWide = new TCanvas("CSCTHitMap","SCT Hit Map",100,100,Can22.xSize,Can22.ySize);
  CanSCTResWide->Divide(2,2);
  
  char hname[80];

  for (Int_t layer=0; layer < 4; layer++) {
    CanSCTResWide->cd(layer+1);
    sprintf(hname,"SCTHits/hSCTHitMapBaLay%d_%d",layer,HistosLevel); 
    DataHis->Draw2D(hname,"substract");
  }

  print_canvas(CanSCTResWide,"SCTBarrelHitMap");
}

////////////////////////////////
void draw_histos_PIXBarrelHitMap() {
  // 

    
  TCanvas *CanPIXBarHitMap = new TCanvas("PIX Barrel Hit Map","PIX Barrel Hit Map (difference)",100,100,1200,400);
  CanPIXBarHitMap->Divide(3,1);
  
  char hname[80];

  for (Int_t layer=0; layer < 3; layer++) {
    CanPIXBarHitMap->cd(layer+1);
    sprintf(hname,"PIXHits/hPIXHitMapBaLay%d_%d",layer,HistosLevel); 
    DataHis->Draw2D(hname,"substract");
  }

  print_canvas(CanPIXBarHitMap,"PIXBarrelHitMap");
}

////////////////////////////////
void draw_histos_PIXBarrelResMap() {
  // 

    
  TCanvas *CanPIXBarResMap = new TCanvas("PIX Barrel Res. Map","PIX Barrel Res. Map (difference)",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarResMap->Divide(3,2);
  
  char hname[80];

  DataHis->SetUserRange(-0.050, 0.050);

  for (Int_t layer=0; layer < 3; layer++) {
    CanPIXBarResMap->cd(layer+1);
    sprintf(hname,"PIXRes/hPIXResMeanMapBaLay%d_%d",layer,HistosLevel); 
    DataHis->Draw2D(hname,"substract");
    //
    CanPIXBarResMap->cd(layer+4);
    sprintf(hname,"PIXRes/hPIXEtaResMeanMapBaLay%d_%d",layer,HistosLevel); 
    DataHis->Draw2D(hname,"substract");
  }

  DataHis->UserRangeOff();

  print_canvas(CanPIXBarResMap,"PIXBarrelResMap");
}

////////////////////////////////
void draw_histos_SCTBarrelResMap() {
  // 

    
  TCanvas *CanSCTBarResMap = new TCanvas("SCT Barrel Res. Map","SCT Barrel Res. Map (difference)",100,100,Can22.xSize,Can22.ySize);
  CanSCTBarResMap->Divide(2,2);
  
  char hname[80];
  Int_t side=0;

  DataHis->SetUserRange(-0.050, 0.050);

  for (Int_t layer=0; layer < 4; layer++) {
    CanSCTBarResMap->cd(layer+1);
    sprintf(hname,"SCTRes/hSCTResMeanMapBaLay%d_s%d_%d",layer,side,HistosLevel); 
    DataHis->Draw2D(hname,"substract");
  }

  DataHis->UserRangeOff();

  print_canvas(CanSCTBarResMap,"SCTBarrelResMap");
}

////////////////////////////////

void draw_histos_ResSCTEndcaps() {
   
   char nameh[80];
  //  Endcap A
   TCanvas *CanSCTEndcapARes = new TCanvas("CanSCTEndcapARes","CanSCTEndcapARes",150,150,1000,1000);
   CanSCTEndcapARes->Divide(3,3);
  
  for(int disc=0;disc<9;disc++){
    CanSCTEndcapARes->cd(disc+1);
    sprintf(nameh,"SCTRes/hSCTResEndcap0Disc%d",disc);
    DrawMe(nameh,"printentries");
    gPad->SetGridx(1);
  }
  print_canvas(CanSCTEndcapARes,"SCTEndcapARes");
  

  //Endcap C
  TCanvas *CanSCTEndcapCRes = new TCanvas("CanSCTEndcapCRes","CanSCTEndcapCRes",150,150,1000,1000);
  CanSCTEndcapCRes->Divide(3,3);
  
  for(int disc=0;disc<9;disc++){
    CanSCTEndcapCRes->cd(disc+1);
    sprintf(nameh,"SCTRes/hSCTResEndcap1Disc%d",disc);
    DrawMe(nameh,"printenetries");
    gPad->SetGridx(1);
  }
  print_canvas(CanSCTEndcapCRes,"SCTEndcapCRes");


}

////////////////////////////////

void draw_histos_ResSCTEndcapsWide() {
 
   char nameh[80];
  //  Endcap A
  TCanvas *CanSCTEndcapAResWide = new TCanvas("SCT EndcapA Resisudals Wide","SCT EndcapA Resisudals Wide",150,150,1000,1000);
  CanSCTEndcapAResWide->Divide(3,3);
  
  for(int disc=0;disc<9;disc++){
    CanSCTEndcapAResWide->cd(disc+1);
    sprintf(nameh,"SCTRes/hSCTResEndcap0Disc%dWide",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
   print_canvas(CanSCTEndcapAResWide,"SCTEndcapAResWide");


  //Endcap C
 TCanvas *CanSCTEndcapCResWide = new TCanvas("SCT EndcapC Resisudals Wide","SCT EndcapC Resisudals Wide",150,150,1000,1000);
 CanSCTEndcapCResWide->Divide(3,3);
  for(int disc=0;disc<9;disc++){
    CanSCTEndcapCResWide->cd(disc+1);
    sprintf(nameh,"SCTRes/hSCTResEndcap1Disc%dWide",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  print_canvas(CanSCTEndcapCResWide,"SCTEndcapCResWide");

}

////////////////////////////////

void draw_histos_PullSCTBarrel() {
  TCanvas *CanSCTPull = new TCanvas("CPullSCTBarrel","CPullSCTBarrel",100,100,Can22.xSize,Can22.ySize);
  CanSCTPull->Divide(2,2);
  CanSCTPull->cd(1);
  
  DrawMe("SCTRes/hSCTPulBaLay0");
  gPad->SetGridx(1);
  
  CanSCTPull->cd(2);
  DrawMe("SCTRes/hSCTPulBaLay1");
  gPad->SetGridx(1);
  
  CanSCTPull->cd(3);
  DrawMe("SCTRes/hSCTPulBaLay2");
  gPad->SetGridx(1);

  CanSCTPull->cd(4);
  DrawMe("SCTRes/hSCTPulBaLay3");
  gPad->SetGridx(1);
  

  print_canvas(CanSCTPull,"PullSCTBarrel");
}

////////////////////////////////

void draw_histos_PullSCTEndcaps() {
  char nameh[80];
  //  Endcap A
  TCanvas *CanSCTEndcapAPull = new TCanvas("CanSCTEndcapAPull","CanSCTEndcapAPull",150,150,1000,1000);
  CanSCTEndcapAPull->Divide(3,3);
  
  for(int disc=0;disc<9;disc++){
    CanSCTEndcapAPull->cd(disc+1);
    sprintf(nameh,"SCTRes/hSCTPulEndcap0Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
   print_canvas(CanSCTEndcapAPull,"PullSCTEndcapA");
  //Endcap C
 TCanvas *CanSCTEndcapCPull = new TCanvas("CanSCTEndcapCPull","CanSCTEndcapCPull",150,150,1000,1000);
 CanSCTEndcapCPull->Divide(3,3);
  for(int disc=0;disc<9;disc++){
    CanSCTEndcapCPull->cd(disc+1);
    sprintf(nameh,"SCTRes/hSCTPulEndcap1Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  print_canvas(CanSCTEndcapCPull,"PullSCTEndcapC");
}

////////////////////////////////
void draw_histos_ResPIXBarrel() {  
  TCanvas *CanPIXBarrelRes = new TCanvas("CResPIXBarrel","PIX Barrel Residuals per layer",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarrelRes->Divide(3,2);
  
  CanPIXBarrelRes->cd(1);
  DrawMe("PIXRes/hPIXPhiResBaLay0");
  //DrawMe("PIXRes/hPIXPhiResBaLay0","PrintMeanAndRMS");
  gPad->SetGridx();

  CanPIXBarrelRes->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1");
  gPad->SetGridx();

  CanPIXBarrelRes->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2");
  gPad->SetGridx();

  CanPIXBarrelRes->cd(4);
  DrawMe("PIXRes/hPIXEtaResBaLay0");
  gPad->SetGridx();

  CanPIXBarrelRes->cd(5);
  DrawMe("PIXRes/hPIXEtaResBaLay1");
  gPad->SetGridx();

  CanPIXBarrelRes->cd(6);
  DrawMe("PIXRes/hPIXEtaResBaLay2");
  gPad->SetGridx();

  print_canvas(CanPIXBarrelRes,"PIXBarrelRes");
}

////////////////////////////////
void draw_histos_PIXBarrelEntries() {  
  TCanvas *CanPIXBarrelEntries = new TCanvas("PIXBarrelEntries","PIX Barrel Entries",100,100,Can22.xSize,Can22.ySize/2);
  CanPIXBarrelEntries->Divide(3,1);
  
  CanPIXBarrelEntries->cd(1);
  DrawMe("PIXRes/hPIXPhiResBaLay0","seriesplot,entries");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelEntries->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1","seriesplot,entries");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelEntries->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2","seriesplot,entries");
  gPad->SetGridx();
  gPad->SetGridy();

  print_canvas(CanPIXBarrelEntries,"PIXBarrelEntries");
}

////////////////////////////////
void draw_histos_PIXBarrelResMean() {  
  TCanvas *CanPIXBarrelResMean = new TCanvas("PIXBarrelResMean","PIX Barrel Residuals Means",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarrelResMean->Divide(3,2);
  
  CanPIXBarrelResMean->cd(1);
  DrawMe("PIXRes/hPIXPhiResBaLay0","seriesplot");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMean->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1","seriesplot");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMean->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2","seriesplot");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMean->cd(4);
  DrawMe("PIXRes/hPIXEtaResBaLay0","seriesplot");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMean->cd(5);
  DrawMe("PIXRes/hPIXEtaResBaLay1","seriesplot");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMean->cd(6);
  DrawMe("PIXRes/hPIXEtaResBaLay2","seriesplot");
  gPad->SetGridx();
  gPad->SetGridy();

  print_canvas(CanPIXBarrelResMean,"PIXBarrelResMean");
}

////////////////////////////////
void draw_histos_PIXBarrelResMeanAndSigma() {  
  TCanvas *CanPIXBarrelResMeanSig = new TCanvas("PIXBarrelResMeanSig","PIX Barrel Residuals Mean and Sigma",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarrelResMeanSig->Divide(3,2);
  
  CanPIXBarrelResMeanSig->cd(1);
  DrawMe("PIXRes/hPIXPhiResBaLay0","seriesplot,MeanAndSigma");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMeanSig->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1","seriesplot,MeanAndSigma");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMeanSig->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2","seriesplot,MeanAndSigma");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMeanSig->cd(4);
  DrawMe("PIXRes/hPIXEtaResBaLay0","seriesplot,MeanAndSigma");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMeanSig->cd(5);
  DrawMe("PIXRes/hPIXEtaResBaLay1","seriesplot,MeanAndSigma");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResMeanSig->cd(6);
  DrawMe("PIXRes/hPIXEtaResBaLay2","seriesplot,MeanAndSigma");
  gPad->SetGridx();
  gPad->SetGridy();

  print_canvas(CanPIXBarrelResMeanSig,"PIXBarrelResMeanAndSigma");
}
////////////////////////////////
void draw_histos_PIXBarrelResByRing() {  
  TCanvas *CanPIXBarrelResByRing = new TCanvas("PIXBarrelResByRing","PIX Barrel residuals by ring",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarrelResByRing->Divide(3,2);
  
  CanPIXBarrelResByRing->cd(1);
  DrawMeProf("PIXRes/hPIXResMeanMapBaLay0");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResByRing->cd(2);
  DrawMeProf("PIXRes/hPIXResMeanMapBaLay1");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResByRing->cd(3);
  DrawMeProf("PIXRes/hPIXResMeanMapBaLay2");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResByRing->cd(4);
  DrawMe("PIXRes/hPIXEtaResBaLay0");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResByRing->cd(5);
  DrawMe("PIXRes/hPIXEtaResBaLay1");
  gPad->SetGridx();
  gPad->SetGridy();

  CanPIXBarrelResByRing->cd(6);
  DrawMe("PIXRes/hPIXEtaResBaLay2");
  gPad->SetGridx();
  gPad->SetGridy();

  print_canvas(CanPIXBarrelResByRing,"PIXBarrelResByRing");
}

////////////////////////////////
void draw_histos_ResPIXBarrelWide() {
  
  TCanvas *CanPIXBarrelResW = new TCanvas("CResPIXBarrelWide","PIX Barrel Residuals per layer (wide)",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarrelResW->Divide(3,2);
  
  CanPIXBarrelResW->cd(1);
  //  DrawMe("PIXRes/hPIXPhiResBaLay0Wide","printmean,nofun");
  //DrawMe("PIXRes/hPIXPhiResBaLay0Wide","printentries");
  DrawMe("PIXRes/hPIXPhiResBaLay0Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResW->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResW->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResW->cd(4);
  DrawMe("PIXRes/hPIXEtaResBaLay0Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResW->cd(5);
  DrawMe("PIXRes/hPIXEtaResBaLay1Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResW->cd(6);
  DrawMe("PIXRes/hPIXEtaResBaLay2Wide");
  gPad->SetGridx(1);

  print_canvas(CanPIXBarrelResW,"PIXBarrelResWide");
}
////////////////////////////////
void draw_histos_ResPIXBarrelClaude() {
  
  TCanvas *CanPIXBarrelResC = new TCanvas("CResPIXBarrelClaude","PIX Barrel Residuals per layer (Claude)",100,100,Can22.xSize,Can22.ySize);
  CanPIXBarrelResC->Divide(3,2);
  
  CanPIXBarrelResC->cd(1);
  //  DrawMe("PIXRes/hPIXPhiResBaLay0Wide","printmean,nofun");
  //DrawMe("PIXRes/hPIXPhiResBaLay0Wide","printentries");
  DrawMe("PIXRes/hPIXPhiResBaLay0");
  gPad->SetGridx(1);

  CanPIXBarrelResC->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1");
  gPad->SetGridx(1);

  CanPIXBarrelResC->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2");
  gPad->SetGridx(1);

  CanPIXBarrelResC->cd(4);
  DrawMe("PIXRes/hPIXEtaResBaLay0Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResC->cd(5);
  DrawMe("PIXRes/hPIXEtaResBaLay1Wide");
  gPad->SetGridx(1);

  CanPIXBarrelResC->cd(6);
  DrawMe("PIXRes/hPIXEtaResBaLay2Wide");
  gPad->SetGridx(1);

  print_canvas(CanPIXBarrelResC,"PIXBarrelResC");
}
////////////////////////////////
void draw_histos_ResPIXBarrelPhi() {
  
  TCanvas *CanPIXBarrelResPhi = new TCanvas("CResPIXBarrelPhi","PIX Barrel Residuals per layer (PHI)",100,100,Can22.xSize,Can22.ySize/2);
  CanPIXBarrelResPhi->Divide(3,1);
  
  CanPIXBarrelResPhi->cd(1);
  //  DrawMe("PIXRes/hPIXPhiResBaLay0Wide","printmean,nofun");
  //DrawMe("PIXRes/hPIXPhiResBaLay0Wide","printentries");
  DrawMe("PIXRes/hPIXPhiResBaLay0");
  gPad->SetGridx(1);

  CanPIXBarrelResPhi->cd(2);
  DrawMe("PIXRes/hPIXPhiResBaLay1");
  gPad->SetGridx(1);

  CanPIXBarrelResPhi->cd(3);
  DrawMe("PIXRes/hPIXPhiResBaLay2");
  gPad->SetGridx(1);


  print_canvas(CanPIXBarrelResPhi,"PIXBarrelResPhi");
}

////////////////////////////////

void draw_histos_ResPIXEndcaps(){
 // 
   char nameh[80];
  //  Endcap A
   TCanvas *CanPIXEndcapARes = new TCanvas("PIX Endcap A Res","PIX Endcap A Res",100,100,Can22.xSize,Can22.ySize);
   CanPIXEndcapARes->Divide(3,2);
  
   for(int disc=0;disc<3;disc++){
     CanPIXEndcapARes->cd(disc+1);
     sprintf(nameh,"PIXRes/hPIXPhiResEndcap0Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }

   for(int disc=0;disc<3;disc++){
     CanPIXEndcapARes->cd(disc+4);
     sprintf(nameh,"PIXRes/hPIXEtaResEndcap0Disc%d",disc);
     DrawMe(nameh);
     gPad->SetGridx(1);
   }
   print_canvas(CanPIXEndcapARes,"ResPIXEndcapA");
   

  //Endcap C
  TCanvas *CanPIXEndcapCRes = new TCanvas("CanPIXEndcapCRes","CanPIXEndcapCRes",100,100,Can22.xSize,Can22.ySize);
  CanPIXEndcapCRes->Divide(3,2);
  
  for(int disc=0;disc<3;disc++){
    CanPIXEndcapCRes->cd(disc+1);
    sprintf(nameh,"PIXRes/hPIXPhiResEndcap1Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  
  for(int disc=0;disc<3;disc++){
    CanPIXEndcapCRes->cd(disc+4);
    sprintf(nameh,"PIXRes/hPIXEtaResEndcap1Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  
  print_canvas(CanPIXEndcapCRes,"ResPIXEndcapC");

}

///////////////////////////////
void draw_histos_SCTBarrelTime() {  
  TCanvas *CanSCTBarrelResTime = new TCanvas("SCTBarrelResTime","SCT Barrel residuals vs time",100,100,Can22.xSize,Can22.ySize);
  
  DrawMeProf("Track_Parameters/hSCTResVsTime");
  gPad->SetGridx();
  gPad->SetGridy();
  

}



void draw_histos_ResPIXEndcapsWide(){
 // 
   char nameh[80];
  //  Endcap A
   TCanvas *CanPIXEndcapAResW = new TCanvas("PIX EndcapA Res Wide","PIX Endcap A ResWide",100,100,Can22.xSize,Can22.ySize);
   CanPIXEndcapAResW->Divide(3,2);
  
   for(int disc=0;disc<3;disc++){
     CanPIXEndcapAResW->cd(disc+1);
     sprintf(nameh,"PIXRes/hPIXPhiResEndcap0Disc%dWide",disc);
     DrawMe(nameh);
     gPad->SetGridx(1);
   }

   for(int disc=0;disc<3;disc++){
     CanPIXEndcapAResW->cd(disc+4);
     sprintf(nameh,"PIXRes/hPIXEtaResEndcap0Disc%dWide",disc);
     DrawMe(nameh);
     gPad->SetGridx(1);
   }
   print_canvas(CanPIXEndcapAResW,"PIXEndcapAResWide");
   

  //Endcap C
  TCanvas *CanPIXEndcapCResW = new TCanvas("PIX Endca p CRes Wide","PIX Endcap C Res Wide",100,100,Can22.xSize,Can22.ySize);
  CanPIXEndcapCResW->Divide(3,2);
  
  for(int disc=0;disc<3;disc++){
    CanPIXEndcapCResW->cd(disc+1);
    sprintf(nameh,"PIXRes/hPIXPhiResEndcap1Disc%dWide",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  
  for(int disc=0;disc<3;disc++){
    CanPIXEndcapCResW->cd(disc+4);
    sprintf(nameh,"PIXRes/hPIXEtaResEndcap1Disc%dWide",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  
  print_canvas(CanPIXEndcapCResW,"PIXEndcapCResWide");

}

///////////////////////////////

void draw_histos_PullPIXBarrel(){
  TCanvas *CanPIXPullBarrel = new TCanvas("CanPIXPullBarrel","CanPIXPullBarrel",100,100,Can22.xSize,Can22.ySize);
  CanPIXPullBarrel->Divide(3,2);
  
  CanPIXPullBarrel->cd(1);
  DrawMe("PIXRes/hPIXPhiPulBaLay0");
  gPad->SetGridx(1);

  CanPIXPullBarrel->cd(2);
  DrawMe("PIXRes/hPIXPhiPulBaLay1");
  gPad->SetGridx(1);

  CanPIXPullBarrel->cd(3);
  DrawMe("PIXRes/hPIXPhiPulBaLay2");
  gPad->SetGridx(1);

  CanPIXPullBarrel->cd(4);
  DrawMe("PIXRes/hPIXEtaPulBaLay0");
  gPad->SetGridx(1);

  CanPIXPullBarrel->cd(5);
  DrawMe("PIXRes/hPIXEtaPulBaLay1");
  gPad->SetGridx(1);

  CanPIXPullBarrel->cd(6);
  DrawMe("PIXRes/hPIXEtaPulBaLay2");
  gPad->SetGridx(1);

  print_canvas(CanPIXPullBarrel,"PullPIXBarrel");

}

///////////////////////////////

void draw_histos_PullPIXEndcaps(){ 
 
   char nameh[80];
  //  Endcap A
   TCanvas *CanPIXEndcapAPull = new TCanvas("CanPIXEndcapAPull","CanPIXEndcapAPull",100,100,Can22.xSize,Can22.ySize);
   CanPIXEndcapAPull->Divide(3,2);
  
   for(int disc=0;disc<3;disc++){
     CanPIXEndcapAPull->cd(disc+1);
     sprintf(nameh,"PIXRes/hPIXPhiPulEndcap0Disc%d",disc);
     DrawMe(nameh);
     gPad->SetGridx(1);
  }

   for(int disc=0;disc<3;disc++){
     CanPIXEndcapAPull->cd(disc+4);
     sprintf(nameh,"PIXRes/hPIXEtaPulEndcap0Disc%d",disc);
     DrawMe(nameh);
     gPad->SetGridx(1);
   }

   print_canvas(CanPIXEndcapAPull,"PullPIXEndcapA");
  

  //Endcap C
  TCanvas *CanPIXEndcapCPull = new TCanvas("CanPIXEndcapCPull","CanPIXEndcapCPull",100,100,Can22.xSize,Can22.ySize);
  CanPIXEndcapCPull->Divide(3,2);
  
  for(int disc=0;disc<3;disc++){
    CanPIXEndcapCPull->cd(disc+1);
    sprintf(nameh,"PIXRes/hPIXPhiPulEndcap1Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
  
   for(int disc=0;disc<3;disc++){
    CanPIXEndcapCPull->cd(disc+4);
    sprintf(nameh,"PIXRes/hPIXEtaPulEndcap1Disc%d",disc);
    DrawMe(nameh);
    gPad->SetGridx(1);
  }
   
  print_canvas(CanPIXEndcapCPull,"PullPIXEndcapC");

}

///////////////////////////////

void draw_histos_ResVsRecoPtSCTBarrel(){
  TCanvas *CanResVsRecoPtSCTBarrel = new TCanvas("ResVsRecoPtSCTBarrel","ResVsRecoPtSCTBarrel",100,100,Can22.xSize,Can22.ySize);
  CanResVsRecoPtSCTBarrel->Divide(2,2);

  CanResVsRecoPtSCTBarrel->cd(1);
  DrawMeProf("Track_Parameters/hResRecoPt_SCTBaLay0");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanResVsRecoPtSCTBarrel->cd(2);
  DrawMeProf("Track_Parameters/hResRecoPt_SCTBaLay1");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanResVsRecoPtSCTBarrel->cd(3);
  DrawMeProf("Track_Parameters/hResRecoPt_SCTBaLay2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanResVsRecoPtSCTBarrel->cd(4);
  DrawMeProf("Track_Parameters/hResRecoPt_SCTBaLay3");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  

  print_canvas(CanResVsRecoPtSCTBarrel,"ResVsRecoPtSCTBarrel");
}

///////////////////////////////
void draw_histos_SystPt() {
  
  char hname[80];
  
  TCanvas *Can7 = new TCanvas("CTest","CTest",225,225,1200,350);
  int nf = DataHis->GetNFiles();
  Can7->Divide(nf,1);
  
  // primer apaguem tots els histos
  for (int i=0; i<nf; i++) {
    DataHis->SwitchOff(i);
  }

  for (int i=0; i<nf; i++) {
    TLegend *TRotllo = new TLegend(0.65,0.6,0.8,0.8,"","NDC");
    // save original color
    int OriginalColor = DataHis->GetLineColor(i);
    //
    Can7->cd(i+1);

    // sols encen el que vol pintar
    DataHis->SwitchOn(i);  
    
    // 
    // i canviem els colors, blau per a les Q>0
    DataHis->SetLineColor(i,4);
    TLine *LinePlus = new TLine(0,1,0,1);
    LinePlus->SetLineColor(4);
    LinePlus->SetLineWidth(2);
    // pintem
    sprintf(hname,"Track_Parameters/hRecoPtQPlus_%d",HistosLevel);
    DataHis->Draw(hname,"new,nolegend,NoTitle,PrintLabelAsTitle");
    sprintf(hname,"trakana/%s",hname);
    TRotllo->AddEntry(LinePlus,"q>0","l");

    // next to draw, new color
    DataHis->SetLineColor(i,2);
    TLine *LineMinus = new TLine(0,1,0,1);
    LineMinus->SetLineColor(2);
    LineMinus->SetLineWidth(2);
    sprintf(hname,"Track_Parameters/hRecoPtQMinus_%d",HistosLevel);
    DataHis->Draw(hname,"same,nolegend");
    sprintf(hname,"trakana/%s",hname);
    TRotllo->AddEntry(LineMinus,"q<0","l");
    DataHis->SwitchOff(i); // el torna a apagar
    // 
    TRotllo->Draw();

    // set back the original color
    DataHis->SetLineColor(i,OriginalColor);
  }


  //
  // - ara els activa tots de nou
  for (int i=0; i<nf; i++) {
    DataHis->SwitchOn(i);
  }

  TCanvas *CanSystPt = new TCanvas("CSystPt","CSystPt",225,225,800,350);
  CanSystPt->Divide(2,1);
  CanSystPt->cd(1);
  DrawMe("Track_Parameters/hRecoPtQPlus");
  CanSystPt->cd(2);
  DrawMe("Track_Parameters/hRecoPtQMinus");

  print_canvas(CanSystPt,"SystPt");
}
///////////////////////////////
void draw_tops_carlos()
{
  TCanvas *Can7 = new TCanvas("CTest","CTest",900,900);
  Can7->Divide(2,2);

  DataHis->SetDirectory("TopViewAna/VKiFi_btag");
  
  Can7->cd(1);
  DrawChangingCuts("h_chi2fit_Mjj_2LJ");

  Can7->cd(2);
  DrawChangingCuts("h_chi2fit_ResEj0_2LJ");

  Can7->cd(3);
  DrawChangingCuts("h_chi2fit_ResEj1_2LJ");

  Can7->cd(4);
  DrawChangingCuts("h_chi2fit_ResMjj_2LJ");

  print_canvas(CTest,"Carlos");
}
///////////////////////////////
void DrawChangingCuts(char nameh[80])
{
  char hname[80];
  int nf = DataHis->GetNFiles();

  // primer apaguem tots els histos
  for (int i=0; i<nf; i++) {
    DataHis->SwitchOff(i);
  }

  for (int i=0; i<nf; i++) {
    // save original color
    int OriginalColor = DataHis->GetLineColor(i);

    // sols encen el que vol pintar
    DataHis->SwitchOn(i);  
    
    // pintem
    sprintf(hname,"%s_%d",nameh,i);
    if (i==0) DataHis->Draw(hname);
    if (i>0) DataHis->Draw(hname,"same");
    // i un colp pintat, l'apaga
    DataHis->SwitchOff(i);  
  }


  //
  // - ara els activa tots de nou
  for (int i=0; i<nf; i++) {
    DataHis->SwitchOn(i);
  }

}

////////////////////////////////

void draw_histos_SystTheta() {
  
  TCanvas *CanSystTheta = new TCanvas("CSystTheta","CSystTheta",225,225,800,350);
  CanSystTheta->Divide(2,1);

  CanSystTheta->cd(1);
  DrawMe("Track_Parameters/hRecoThetaFwd");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanSystTheta->cd(2);
  DrawMe("Track_Parameters/hRecoThetaBwd");
  gPad->SetGridx(1);  
  gPad->SetGridy(1);

  print_canvas(CanSystTheta,"SystTheta");
}

////////////////////////////////

void draw_histos_QoverPt() {
  // 
  TCanvas *CanQoverP = new TCanvas("CQoverP","CQOverP",50,50,1000,700);
  CanQoverP->Divide(3,2);
  
  CanQoverP->cd(1);
  DrawMe("Track_Parameters/hRecoQoverPt");

  CanQoverP->cd(2);
  DataHis->DrawProf("Track_Parameters/hRecoQoverPt_RecoPhi0_QPlus_1");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanQoverP->cd(3);
  DataHis->DrawProf("Track_Parameters/hRecoQoverPt_RecoPhi0_QMinus_1");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanQoverP->cd(4);
  DrawMe("Track_Parameters/hRecoQoverPt_TruzQoverPt");
  gPad->SetGridx(1);

 CanQoverP->cd(5);
  DrawMe("Track_Parameters/hRecoQoverPt_TruzQoverPt_QPlus");
  gPad->SetGridx(1);

  CanQoverP->cd(6);
  DrawMe("Track_Parameters/hRecoQoverPt_TruzQoverPt_QMinus");
  gPad->SetGridx(1);

  print_canvas(CanQoverP,"QoverP");
}

////////////////////////////////

void draw_histos_D0() 
{
  // 
  TCanvas *CanD0 = new TCanvas("CD0","CD0",100,100,Can22.xSize,Can22.ySize);
  CanD0->Divide(2,2);
  
  CanD0->cd(1);
  DrawMe("Track_Parameters/hRecoD0Zoom");
  gPad->SetGridx(1);

  CanD0->cd(2);
  DrawMe("Track_Parameters/hRecoD0Wide");
  gPad->SetGridx(1);
  
  CanD0->cd(3);
  DrawMe("Track_Parameters/hRecodD0");
  gPad->SetGridx(1);

  CanD0->cd(4);
  DrawMeProf("Track_Parameters/hRecoD0_RecoPhi0");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(CanD0,"D0");

  //
  TCanvas *CanD0_2 = new TCanvas("CD0_2","CD0_2",100,100,Can22.xSize,Can22.ySize);
  CanD0_2->Divide(2,2);
  
  CanD0_2->cd(1);
  //DrawMeProf("hRecodD0_RecoPhi0_Narrow");
  DrawMeProf("Track_Parameters/hRecodD0_RecoPhi0");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanD0_2->cd(2);
  DrawMeProf("Track_Parameters/hRecodD0_RecoPt");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanD0_2->cd(3);
  DrawMe("Track_Parameters/hRecodD0Signif");
    
  CanD0_2->cd(4);
  DrawMe("Track_Parameters/hRecodD0SignifWide");
  gPad->SetGridx(1);


  print_canvas(CanD0_2,"D0_2");
}

////////////////////////////////
void draw_histos_SCTHitError() {
  // 
  TCanvas *CanSCTErr = new TCanvas("SCT Hit Error","SCT Hit Error",100,100,Can22.xSize,Can22.ySize);
  CanSCTErr->Divide(2,2);
  
  CanSCTErr->cd(1);
  DrawMe("SCTRes/hSCTErrBaLay0");
  gPad->SetGridx(1);
  
  CanSCTErr->cd(2);
  DrawMe("SCTRes/hSCTErrBaLay1");
  gPad->SetGridx(1);
  
  CanSCTErr->cd(3);
  DrawMe("SCTRes/hSCTErrBaLay2");
  gPad->SetGridx(1);
  
  CanSCTErr->cd(4);
  DrawMe("SCTRes/hSCTErrBaLay3");
  gPad->SetGridx(1);
  

  print_canvas(CanSCTErr,"SCTHitError");
}

////////////////////////////////
void draw_histos_PIXHitError() {
  // 
  TCanvas *CanPIXErr = new TCanvas("PIX Hit Error","PIX Hit Error",100,100,Can22.xSize,Can22.ySize);
  CanPIXErr->Divide(3,2);
  
  CanPIXErr->cd(1);
  DrawMe("PIXRes/hPIXErrPhiBaLay0");
  gPad->SetGridx(1);
  
  CanPIXErr->cd(2);
  DrawMe("PIXRes/hPIXErrPhiBaLay1");
  gPad->SetGridx(1);
  
  CanPIXErr->cd(3);
  DrawMe("PIXRes/hPIXErrPhiBaLay2");
  gPad->SetGridx(1);
  
  CanPIXErr->cd(4);
  DrawMe("PIXRes/hPIXErrEtaBaLay0");
  gPad->SetGridx(1);
  
  CanPIXErr->cd(5);
  DrawMe("PIXRes/hPIXErrEtaBaLay1");
  gPad->SetGridx(1);
  
  CanPIXErr->cd(6);
  DrawMe("PIXRes/hPIXErrEtaBaLay2");
  gPad->SetGridx(1);
  
  print_canvas(CanPIXErr,"PIXHitError");
}

////////////////////////////////

void draw_histos_Chi2() {
  // 
  TCanvas *CanChi2 = new TCanvas("Chi2","Chi2",100,100,Can22.xSize,Can22.ySize);
  CanChi2->Divide(2,2);
  
  CanChi2->cd(1);
  if (DataHis->HExists("hRecoChi2Wide_0")) {
    //DrawMe("Track_Parameters/hRecoChi2Wide");
    DrawMe("Track_Parameters/hRecoChi2Wide");
  }
  else {
    DrawMe("Track_Parameters/hRecoChi2");
  }
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  CanChi2->cd(1);

  CanChi2->cd(2);
  if (DataHis->HExists("hRecoChi2Wide_0")) {
    DrawMe("Track_Parameters/hRecoChi2Wide","seriesplot");
  }
  else {
    DrawMe("Track_Parameters/hRecoChi2","seriesplot");
  }
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanChi2->cd(3);
  DrawMeProf("Track_Parameters/hRecoChi2_RecoPhi0");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanChi2->cd(4);
  DrawMeProf("Track_Parameters/hRecoChi2_RecoEta");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(CanChi2,"Chi2Histos");
}

////////////////////////////////

void draw_histos_Chi2Evolution() {
  TCanvas *CanBigChi2 = new TCanvas("BigChi2","BigChi2",100,100,Can22.xSize,Can22.ySize);
  DrawMe("Track_Parameters/hRecoChi2Wide","seriesplot");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  print_canvas(CanBigChi2,"Chi2Evolution");
}

void draw_histos_PtPhi0() {
  
  TCanvas *CanPtPhi0 = new TCanvas("Pt vs Phi0","Pt vs Phi0 (prof)",100,100,600,800);
  CanPtPhi0->Divide(1,2);
  
  char hname[80];

  CanPtPhi0->cd(1);
  sprintf(hname,"Track_Parameters/hRecoPt_RecoPhi0_QPlus_%d",HistosLevel);
  DataHis->DrawProf(hname);
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanPtPhi0->cd(2);
  sprintf(hname,"Track_Parameters/hRecoPt_RecoPhi0_QMinus_%d",HistosLevel);
  DataHis->DrawProf(hname);
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(CanPtPhi0,"PtvsPhi0_prof");
}

////////////////////////////////
void draw_histos_Pt_Phi0Eta() {
  TCanvas *CanPt_Phi0Eta = new TCanvas("Pt vs Phi0 and Eta","Pt vs Phi0 (Eta) diff",100,100,600,800);
  CanPt_Phi0Eta->Divide(1,2);

  char hname[80];

  CanPt_Phi0Eta->cd(1);
  sprintf(hname,"Track_Parameters/hRecoPt_RecoPhi0_%d",HistosLevel); 
  DataHis->Draw2D(hname,"substract");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  CanPt_Phi0Eta->cd(2);
  sprintf(hname,"Track_Parameters/hRecoPt_RecoEta_%d",HistosLevel); 
  DataHis->Draw2D(hname,"substract");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(CanPt_Phi0Eta,"Pt_Phi0_Eta");  
}

////////////////////////////////

void draw_histos_mass() {
  //
  TCanvas *CanZMass = new TCanvas("CZMass","CZMass",100,100,Can22.xSize,Can22.ySize);
  CanZMass->Divide(2,2);
  
  CanZMass->cd(1);
  DrawMe("mass_analysis/hMuonRecoPt","printaverage");

  CanZMass->cd(2);
  DrawMe("mass_analysis/hMuonPairMass");

  CanZMass->cd(3);
  DrawMe("mass_analysis/hMuonPairPt","printentries");

  CanZMass->cd(4);
  DrawMe("mass_analysis/hMuonPairDPhi");

  print_canvas(CanZMass,"ZMass");
}

////////////////////////////////
void draw_histos_Pt() {
  // raw Pt
  //TCanvas *CanPt = new TCanvas("CPt","CPt",50,50,900,425);
  TCanvas *CanPt = new TCanvas("PT Reco","PT Reco",50,50,900,900);
  CanPt->Divide(1,1);
  
  CanPt->cd(1);
  DrawMe("Track_Parameters/hRecoPt");
  gPad->SetGridx(1);
  gPad->SetGridy(1);


  //CanPt->cd(2);
  //DrawMe("Track_Parameters/hRecoQoverPt_TruzQoverPt");
  //gPad->SetGridx(1);
  //gPad->SetGridy(1);

  print_canvas(CanPt,"Pt");
}

////////////////////////////////
void draw_histos_TrkMatching()
{
  TCanvas *CanTrkMatching = new TCanvas("CTrkMatching","CTrkMatching",100,100,Can22.xSize,Can22.ySize);
  CanTrkMatching->Divide(2,2);
  
  CanTrkMatching->cd(1);
  DataHis->Draw("TrkMatching/hTrkMatching_D0_2");

  CanTrkMatching->cd(2);
  DataHis->Draw("TrkMatching/hTrkMatching_Z0_2");

  CanTrkMatching->cd(3);
  DataHis->Draw("TrkMatching/hTrkMatching_Phi0_2");

  CanTrkMatching->cd(4);
  DataHis->Draw("TrkMatching/hTrkMatching_Eta_2");

  print_canvas(CanTrkMatching,"TrkMatching");
}

void draw_histos_TrkMatching_Profiles_Vs_D0(){

  //Track Matching (up-low) Parameter Comparation Vs D0
  TCanvas * cCompTrackMatchingParamsVsD0 = new TCanvas("CompTrkMatchingParamsVsd0","Track Matching (up-low) Parameter Comparation Vs D0",1000,700);
  
  cCompTrackMatchingParamsVsD0->Divide(3,2);
  
  cCompTrackMatchingParamsVsD0->cd(1);
  DataHis->DrawProf("TrkMatching/hTrkMatching_D0_Vs_D02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  cCompTrackMatchingParamsVsD0->cd(2);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Z0_Vs_D02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsD0->cd(3);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Phi0_Vs_D02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsD0->cd(4);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Theta0_Vs_D02");
  gPad->SetGridx(1);
   gPad->SetGridy(1);

  cCompTrackMatchingParamsVsD0->cd(5);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Eta_Vs_D02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsD0->cd(6);
  DataHis->DrawProf("TrkMatching/hTrkMatching_QoverPt_Vs_D02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(cCompTrackMatchingParamsVsD0,"CompTrkMatchingParamsVsd0");
}

void draw_histos_TrkMatching_Profiles_Vs_Z0(){
  //Track Matching (up-low) Parameter Comparation Vs Z0
  TCanvas * cCompTrackMatchingParamsVsZ0 = new TCanvas("CompTrkMatchingParamsVsz0","Track Matching (up-low) Parameter Comparation Vs Z0",1000,700);
 
  cCompTrackMatchingParamsVsZ0->Divide(3,2);
  
  cCompTrackMatchingParamsVsZ0->cd(1);
  DataHis->DrawProf("TrkMatching/hTrkMatching_D0_Vs_Z2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  cCompTrackMatchingParamsVsZ0->cd(2);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Z0_Vs_Z2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsZ0->cd(3);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Phi0_Vs_Z2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsZ0->cd(4);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Theta0_Vs_Z2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
   
  cCompTrackMatchingParamsVsZ0->cd(5);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Eta_Vs_Z2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsZ0->cd(6);
  DataHis->DrawProf("TrkMatching/hTrkMatching_QoverPt_Vs_Z2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(cCompTrackMatchingParamsVsZ0,"CompTrkMatchingParamsVsz0");

}

void draw_histos_TrkMatching_Profiles_Vs_Phi0(){
  //Track Matching (up-low) Parameter Comparation Vs Phi0
  TCanvas * cCompTrackMatchingParamsVsPhi0 = new TCanvas("CompTrkMatchingParamsVsphi0","Track Matching (up-low) Parameter Comparation Vs Phi0",1000,700);
 
  cCompTrackMatchingParamsVsPhi0->Divide(3,2);
  
  cCompTrackMatchingParamsVsPhi0->cd(1);
  DataHis->DrawProf("TrkMatching/hTrkMatching_D0_Vs_Phi02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  cCompTrackMatchingParamsVsPhi0->cd(2);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Z0_Vs_Phi02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsPhi0->cd(3);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Phi0_Vs_Phi02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsPhi0->cd(4);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Theta0_Vs_Phi02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
   
  cCompTrackMatchingParamsVsPhi0->cd(5);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Eta_Vs_Phi02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsPhi0->cd(6);
  DataHis->DrawProf("TrkMatching/hTrkMatching_QoverPt_Vs_Phi02");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(cCompTrackMatchingParamsVsPhi0,"CompTrkMatchingParamsVsphi0");

}

void draw_histos_TrkMatching_Profiles_Vs_Eta(){
  //Track Matching (up-low) Parameter Comparation Vs eta0
  TCanvas * cCompTrackMatchingParamsVsEta = new TCanvas("CompTrkMatchingParamsVseta","Track Matching (up-low) Parameter Comparation Vs eta",1000,700);
 
  cCompTrackMatchingParamsVsEta->Divide(3,2);
  
  cCompTrackMatchingParamsVsEta->cd(1);
  DataHis->DrawProf("TrkMatching/hTrkMatching_D0_Vs_Eta2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  cCompTrackMatchingParamsVsEta->cd(2);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Z0_Vs_Eta2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsEta->cd(3);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Phi0_Vs_Eta2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsEta->cd(4);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Theta0_Vs_Eta2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
   
  cCompTrackMatchingParamsVsEta->cd(5);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Eta_Vs_Eta2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsEta->cd(6);
  DataHis->DrawProf("TrkMatching/hTrkMatching_QoverPt_Vs_Eta2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(cCompTrackMatchingParamsVsEta,"CompTrkMatchingParamsVseta");

}

void draw_histos_TrkMatching_Profiles_Vs_QoverPt(){
  //Track Matching (up-low) Parameter Comparation Vs eta0
  TCanvas * cCompTrackMatchingParamsVsQoverPt = new TCanvas("CompTrkMatchingParamsVsQoverPt","Track Matching (up-low) Parameter Comparation Vs QoverPt",1000,700);
 
  cCompTrackMatchingParamsVsQoverPt->Divide(3,2);
  
  cCompTrackMatchingParamsVsQoverPt->cd(1);
  DataHis->DrawProf("TrkMatching/hTrkMatching_D0_Vs_QoverPt2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  
  cCompTrackMatchingParamsVsQoverPt->cd(2);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Z0_Vs_QoverPt2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsQoverPt->cd(3);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Phi0_Vs_QoverPt2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsQoverPt->cd(4);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Theta0_Vs_QoverPt2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
   
  cCompTrackMatchingParamsVsQoverPt->cd(5);
  DataHis->DrawProf("TrkMatching/hTrkMatching_Eta_Vs_QoverPt2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  cCompTrackMatchingParamsVsQoverPt->cd(6);
  DataHis->DrawProf("TrkMatching/hTrkMatching_QoverPt_Vs_QoverPt2");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  print_canvas(cCompTrackMatchingParamsVsQoverPt,"CompTrkMatchingParamsVsQoverPt");

}


////////////////////////////////
void draw_histos_custom(){
TCanvas *CanCustom = new TCanvas("CanCustom","CanCustom",225,225,1200,350);
  CanCustom->Divide(3,1);
  
  CanCustom->cd(1);
  DrawMe("Track_Parameters/hRecoPt");
  gPad->SetGridx(1);

  CanCustom->cd(2);
  DrawMe("PIXRes/hPIXPhiResBa");
  gPad->SetGridx(1);

  CanCustom->cd(3);
  DrawMe("SCTRes/hSCTResBa");
  gPad->SetGridx(1);

  print_canvas(CanCustom,"Custom");
}

///////////////////////////////

void print_canvas(TCanvas *ThisCanvas, char *ThisName) 
{
  sprintf(CanvasName,"%s/%s__%s.%s",GeneralName,GeneralName,ThisName,ImageType);
  ThisCanvas->Print(CanvasName);
}

////////////////////////////////

void DrawMe(char *htodraw, char *xopt )
{
  char Histo_ID[80];
  sprintf(Histo_ID,"%s_%d",htodraw,HistosLevel);
  // cout << " -- DrawMe -- " << Histo_ID << endl;
  DataHis->Draw(Histo_ID,xopt);
  //setTitleFont(42);
}

////////////////////////////////
void DrawMeProf(char *htodraw, char *xopt )
{
  char Histo_ID[80];
  sprintf(Histo_ID,"%s_%d",htodraw,HistosLevel);
  // cout << " -- DrawMe -- " << Histo_ID << endl;
  DataHis->DrawProf(Histo_ID,xopt);
}

////////////////////////////////
void draw_histos_HitsPerLayer() {
  // raw Pt
  TCanvas *CanHitsPerLayer = new TCanvas("HITs per layer","HITs per layer",50,50,Can22.xSize,Can22.ySize);
  CanHitsPerLayer->Divide(3,2);
  
  CanHitsPerLayer->cd(1);
  DrawMe("PIXHits/hPIXEndcap1Hits","nolegend");
  gPad->SetGridx(1);
  CanHitsPerLayer->cd(2);
  DrawMe("PIXHits/hPIXBarrelHits","nolegend");
  gPad->SetGridx(1);
  CanHitsPerLayer->cd(3);
  DrawMe("PIXHits/hPIXEndcap0Hits","nolegend");
  gPad->SetGridx(1);

  CanHitsPerLayer->cd(4);
  DrawMe("SCTHits/hSCTEndcap1Hits","nolegend");
  gPad->SetGridx(1);
  CanHitsPerLayer->cd(5);
  DrawMe("SCTHits/hSCTBarrelHits","nolegend");
  gPad->SetGridx(1);
  CanHitsPerLayer->cd(6);
  DrawMe("SCTHits/hSCTEndcap0Hits","nolegend");
  gPad->SetGridx(1);

  print_canvas(CanHitsPerLayer,"HITsPerLayer");
}

////////////////////////////////
void load_CanvasTypes() {
  Can22.xSize=1200;
  Can22.ySize=800;
}

////////////////////////////////
void Print_StatsTable(char *htodraw){
  
  char Histo_ID[80];
  sprintf(Histo_ID,"%s_%d",htodraw,HistosLevel);
  
  cout << " ** Statistics table ** " <<endl;
  DataHis->PrintStatsTable(Histo_ID);
  cout << " ** ---------------- ** " <<endl;
}
  

