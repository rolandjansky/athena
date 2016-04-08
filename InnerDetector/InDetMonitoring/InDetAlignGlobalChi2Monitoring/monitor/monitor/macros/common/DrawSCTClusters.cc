/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////
void DrawSCTClusters(char nname[80], int events=0)
//////////////////////////////////////////////////
{  
  TFile *f0 = new TFile(nname);
  SiClusterNtu = (TTree*)f0->Get("SiCluster");

  TCanvas *CBarrelXY = new TCanvas("BarrelXY","BarrelXY",800,800);
  CBarrelXY->Divide(1,1);

  CBarrelXY->cd(1);
  TH2F *marc = new TH2F ("marc","SCT clusters", 20, -600, 600, 20, -600, 600);
  marc->SetStats(kFALSE);
  marc->GetXaxis()->SetTitle("X (mm)");
  marc->GetXaxis()->SetTitleSize(0.035);
  marc->GetXaxis()->SetLabelSize(0.03);

  marc->GetYaxis()->SetTitle("Y (mm)");
  marc->GetYaxis()->SetTitleSize(0.036);
  marc->GetYaxis()->SetTitleOffset(1.2);
  marc->GetYaxis()->SetLabelSize(0.03);

  marc->Draw();
  SiClusterNtu->Draw("sct_clus_y:sct_clus_x","","same");
  
  CBarrelXY->Print("SCT_clusters.gif");
  return;
}
