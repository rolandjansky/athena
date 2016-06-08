/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void tile_cluster() {

  TString path="/Tile/Cluster";

  file->cd(path);

  //===== switch for outputs =====
  //  bool gifOn=false; 
  //  bool epsOn=true;
  
  gROOT->SetStyle("Plain");
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadTopMargin   (0.20);
  gStyle->SetStatFontSize  (0.05);
  gStyle->SetStatStyle     (0);
  gStyle->SetOptStat(111111);  
  gStyle->SetOptFit(1100);  
  gStyle->SetTitleH         (0.1);
  gStyle->SetCanvasDefW(900);
  gStyle->SetCanvasDefH(600);
  gStyle->SetPalette(1);



 

  TString StrNames[13];
  StrNames[0]= "tilenClusters"; StrNames[1]= "tileClusternCells" ; StrNames[2]= "tileAllClusterEnergy";
  StrNames[3]="tileClusterEt";   StrNames[4]= "tileClusterEnergy";  StrNames[5]= "tileClusterEnergyOvThr";
  StrNames[6]= "tileClusterTime"; 
  StrNames[7]= "tileClusterEta"; StrNames[8]= "tileClusterPhi"; StrNames[9]= "tileClusterEtaPhi";
  StrNames[10]= "tileClusterEtaPhiDiff"; StrNames[11]="tileClusterEneDiff"; StrNames[12]="tileClusterTimeDiff"; 


  int index;
  TCanvas *c_clustergen=new TCanvas("c_clustergen","Tile Cluster General");
  c_clustergen->Divide(1,2);

  for (int i=0;i<2;i++) {

      index=i+1;
      c_clustergen->cd(index);

      TH1D* clustergen = (TH1D*)file->Get( path+"/"+StrNames[i] );  
      //      cout << "cellsamp=" << cellsamp <<"\tcellsamp->GetTitle()=" << cellsamp->GetTitle();

      clustergen->SetFillColor(38); 
      clustergen->Draw("histo");
      c_clustergen->Update();

      gSystem->Sleep(200);
   
  }


  TCanvas *c_clusterene=new TCanvas("c_clusterene","Tile Cluster histograms");
  c_clusterene->Divide(2,3);

  for (int i=2;i<7;i++) {
    index=i-1;
    c_clusterene->cd(index);
    TH1D* clusterene =  (TH1D*)file->Get( path+"/"+StrNames[i] ); 
    clusterene->SetFillColor(38); if (i!=6 ) {gPad->SetLogy(1);}
    clusterene->Draw("histo");
    c_clusterene->Update();
    gSystem->Sleep(200);

  }

  TCanvas *c_clusterpos=new TCanvas("c_clusterpos","Tile Cluster Position histograms");
  c_clusterpos->Divide(2,2);

  for (int i=7;i<10;i++) {
    index=i-6;
    c_clusterpos->cd(index);
    TH1D* clusterpos =  (TH1D*)file->Get( path+"/"+StrNames[i] );    
    if (i==9)  {clusterpos->Draw("zcol");}
    else {clusterpos->SetFillColor(38); clusterpos->Draw("histo");}
    c_clusterpos->Update();
    gSystem->Sleep(200);
  }

  TCanvas *c_clustercor=new TCanvas("c_clustercor","Tile Cluster Correlation histograms");
  c_clustercor->Divide(2,2);

  for (int i=10;i<13;i++) {
    index=i-9;
    c_clustercor->cd(index);
    TH1D* clustercor =  (TH1D*)file->Get( path+"/"+StrNames[i] );    
    if (i==10)  {clustercor->Draw("zcol");}
    else {clustercor->SetFillColor(38); clustercor->Draw("histo");}
    c_clustercor->Update();
    gSystem->Sleep(200);
  }

}





