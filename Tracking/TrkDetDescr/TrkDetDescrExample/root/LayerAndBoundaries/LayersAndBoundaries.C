/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void LayersAndBoundaries(TString& TGfile, bool calo=false) {

// get the File
TFile* file = TFile::Open(TGfile, "READ");

gStyle->SetOptStat(0);


if (file){
  TTree* treeNavigator = (TTree*)file->Get("NavigatorValidation");
  TTree* treeMapper = (TTree*)file->Get("MatUpdatorValidation");

  double boundLimitR = calo ? 5000. : 1200.;
  double boundLimitZ = calo ? 7000. : 4000.;
  

  if (treeNavigator && treeMapper){

    TCanvas* zRView = new TCanvas("zrView", "TrackingGeometry z-r projection", 100, 100, 800, 600);
    zRView->cd();
    
        TString rzLimitString  = "";
            rzLimitString += -boundLimitZ;
	    rzLimitString += ",";
	    rzLimitString += boundLimitZ;
	    rzLimitString += ", 700,";
	    rzLimitString += "0., ";
	    rzLimitString += boundLimitR;
	    rzLimitString += ")";
    
     
    treeNavigator->Draw("BoundaryHitR:BoundaryHitZ>>Boundaries(700,"+rzLimitString,"","", 20000, 0);
    treeMapper->Draw("UpdatePositionR:UpdatePositionZ>>Layers(700,"+rzLimitString,"","", 20000, 0);

    TH2F* boundaries = (TH2F*)gDirectory->Get("Boundaries");
    TH2F* layers = (TH2F*)gDirectory->Get("Layers");

    boundaries->SetMarkerColor(2);
    boundaries->SetMarkerSize(0.5);
    boundaries->Draw();
    layers->Draw("same");

    // the eta lines
    
    
    TLine* eta000 = new TLine(0.,0.,0.,boundLimitR);
    TLine* eta025 = new TLine(0.,0.,boundLimitR/tan(1.32336),boundLimitR);
    TLine* eta050 = new TLine(0.,0.,boundLimitR/tan(1.09042),boundLimitR);
    TLine* eta075 = new TLine(0.,0.,boundLimitR/tan(0.88256),boundLimitR);
    TLine* eta100 = new TLine(0.,0.,boundLimitR/tan(0.70502),boundLimitR);
    TLine* eta150 = new TLine(0.,0.,boundLimitR/tan(0.4390),boundLimitR);
    TLine* eta200 = new TLine(0.,0.,boundLimitR/tan(0.269),boundLimitR);
    TLine* eta250 = new TLine(0.,0.,boundLimitR/tan(0.1638),boundLimitR);
    TLine* eta300 = new TLine(0.,0.,boundLimitR/tan(0.099492),boundLimitR);
   
   
    eta000->SetLineColor(4);
    eta025->SetLineColor(4);
    eta050->SetLineColor(4);
    eta075->SetLineColor(4);
    eta100->SetLineColor(4);
    eta150->SetLineColor(4);
    eta200->SetLineColor(4);
    eta250->SetLineColor(4);
    eta300->SetLineColor(4);
     
    eta000->Draw();
    eta025->Draw();
    eta050->Draw();
    eta075->Draw();
    eta100->Draw();
    eta150->Draw();
    eta200->Draw();
    eta250->Draw();
    eta300->Draw();
    

    TCanvas* xyView_Barrel = new TCanvas("xyView_Barrel", "TrackingGeometry x-y projection / Barrel", 200, 200, 600, 620);
    xyView_Barrel->cd();

    TString xyLimitString  = "";
            xyLimitString += -boundLimitR;
	    xyLimitString += ",";
	    xyLimitString += boundLimitR;
	    xyLimitString += ", 700,";
	    xyLimitString += -boundLimitR;
	    xyLimitString += ",";
	    xyLimitString += boundLimitR;
	    xyLimitString += ")";
	       
    
treeMapper->Draw("UpdatePositionY:UpdatePositionX>>HitBarrel(700,"+xyLimitString,"abs(UpdatePositionZ)<300","", 
20000, 0);
    treeNavigator->Draw("BoundaryHitY:BoundaryHitX>>BoundariesBarrel(700,"+xyLimitString,"abs(BoundaryHitZ)<300","", 20000, 
0);

    TH2F* boundaries_b = (TH2F*)gDirectory->Get("BoundariesBarrel");
    TH2F* layers_b = (TH2F*)gDirectory->Get("HitBarrel");

    boundaries_b->SetMarkerColor(2);
    boundaries_b->SetMarkerSize(0.5);
    boundaries_b->Draw();
    layers_b->Draw("same");

   }
  }

}
