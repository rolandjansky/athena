/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{   
   
   gStyle->SetTitleW(0.5);
   gStyle->SetTitleH(0.08);
   gStyle->SetTitleX(0.13);
   gStyle->SetTitleY(0.98);
   gStyle->SetStatX(0.98);
   gStyle->SetStatY(0.88);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.15);
   
   
      
 //  TFile AAN1("../run/tteff.histos.root", "READ");   
   TFile histofile1("../run/muonhits3.root", "READ");

 //  TFile AAN2("./tteff.histos.root", "READ");
//   TFile histofile2("../root/muonhits3.root", "READ");  
   
//  //  TFile AAN2("./tteff.histos.root", "READ");
//    TFile histofile3("../root/muonhits2.root", "READ");
   
   TCanvas *c1 = new TCanvas("c1","MDT Validation Plots",0,0,1000,500);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
  
    c1->Divide(1,1);   
//     c1->cd(1);      
//     TTree* MDTStationName1 = (TTree*)AAN1.Get ("MDTHitsValidation/MDTTestHits");
//     MDTStationName1->SetLineColor(1);   
//     MDTStationName1->Draw("m_Validation_MDT_StationName");
//     TTree* MDTStationName2 = (TTree*)AAN2.Get ("MDTHitsValidation/MDTTestHits");
//     MDTStationName2->SetLineColor(2);   
//     MDTStationName2->Draw("m_Validation_MDT_StationName","","same");
// 
//        
//     c1->cd(2);      
//     TTree* MDTStationEta1 = (TTree*)AAN1.Get ("MDTHitsValidation/MDTTestHits");
//     MDTStationEta1->SetLineColor(1);   
//     MDTStationEta1->Draw("m_Validation_MDT_StationName");
//     TTree* MDTStationEta2 = (TTree*)AAN2.Get ("MDTHitsValidation/MDTTestHits");
//     MDTStationEta2->SetLineColor(2);   
//     MDTStationEta2->Draw("m_Validation_MDT_StationEta","","same");
// 
// 
//     c1->cd(3);      
//     TTree* MDTStationPhi1 = (TTree*)AAN1.Get ("MDTHitsValidation/MDTTestHits");
//     MDTStationPhi1->SetLineColor(1);   
//     MDTStationPhi1->Draw("m_Validation_MDT_StationName");
//     TTree* MDTStationPhi2 = (TTree*)AAN2.Get ("MDTHitsValidation/MDTTestHits");
//     MDTStationPhi2->SetLineColor(2);   
//     MDTStationPhi2->Draw("m_Validation_MDT_StationPhi","","same");


    c1->cd(1);   	  
    TH1F *ChamberOcc1 = (TH1F*) histofile1.Get("MDT/;1");
    ChamberOcc1->SetTitle("Hit Occupancy per Chamber");
    ChamberOcc1->Sumw2();
    ChamberOcc1->SetLineColor(1);
    ChamberOcc1->SetFillColor(0);
    ChamberOcc1->Draw();
//     TH1F *ChamberOcc2 = (TH1F*) histofile2.Get("MDT/;1");
//     ChamberOcc2->SetLineColor(2);
//     ChamberOcc2->SetFillColor(0);
//     ChamberOcc2->Sumw2();
//     ChamberOcc2->Draw("same");
//     TH1F *ChamberOcc3 = (TH1F*) histofile3.Get("MDT/;1");
//     ChamberOcc3->SetLineColor(4);
//     ChamberOcc3->SetFillColor(0);
//     ChamberOcc3->Sumw2();
//     ChamberOcc3->Draw("same");
}
