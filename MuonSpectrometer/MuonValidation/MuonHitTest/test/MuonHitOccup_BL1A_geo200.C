/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{     
#include <sstream>   
#include <string.h>

//give chamber name:
  std::string secname="BL1A";
  std::string tagname="geo200";
  std::string tagname1="geo200";
  std::string rel="14223";
  std::string type="muminus_pt100GeV";
  std::string sample="rfio:/castor/cern.ch/user/m/muonprod/MuonValidation/"+tagname+"/MuonHitsVal_"+secname+"_"+tagname1+"_"+rel+"_"+type+".histo.root";  
  const char* sample_char=sample.c_str();
  //cout << "sample_char : " <<  sample_char << endl;
   
   gStyle->SetTitleW(0.5);
   gStyle->SetTitleH(0.08);
   gStyle->SetTitleX(0.13);
   gStyle->SetTitleY(0.98);
   gStyle->SetStatX(0.98);
   gStyle->SetStatY(0.88);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.15);
   
 TFile *histofile1= TFile::Open("rfio:/castor/cern.ch/user/m/muonprod/MuonValidation/geo200/MuonHitsVal_BL1A_geo200_14223_muminus_pt100GeV.histo.root", "READ");    
  // TFile *histofile1= TFile::Open(+sample_char+, "READ");    

   TCanvas *HitOccupancyperChamber_BL1A_csc100 = new TCanvas("HitOccupancyperChamber_BL1A_geo200","MDT Validation Plots_BL1A_geo200",0,0,1000,500);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
    
   HitOccupancyperChamber_BL1A_csc100->Divide(1,1);   
   HitOccupancyperChamber_BL1A_csc100->cd(1);   	  
   TH1F *ChamberOcc1 = (TH1F*) histofile1.Get("MDT/;1");
   ChamberOcc1->SetTitle("Hit Occupancy per Chamber in BL1A in geo200");
   ChamberOcc1->Sumw2();
   ChamberOcc1->SetLineColor(1);
   ChamberOcc1->SetFillColor(0);
   ChamberOcc1->Draw();
   HitOccupancyperChamber_BL1A_csc100->Modified();
   HitOccupancyperChamber_BL1A_csc100->cd();
   HitOccupancyperChamber_BL1A_csc100->SetSelected(HitOccupancyperChamber_BL1A_csc100);
   HitOccupancyperChamber_BL1A_csc100->SaveAs("HitOccupancyperChamber_BL1A_geo200.RTT.ps");
}
