/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=====================================================================
//  hitmapsXY
//=====================================================================

const int iiNSCT_Layers = 4;
TH2F *hSctBa[iiNSCT_Layers]; 

//=====================================================================
//  hitmapsXY
//=====================================================================
void hitmapsXY() {

  for(int layer = 0; layer<iiNSCT_Layers; layer++){
    sprintf(name,"trkana/SCTHits/hSCTHitMapBaLay%d_%d",layer,1);
    hSctBa[layer] = (TH2F*)file->Get(name);
  }

  showHitmapXY();
  return;
}


//=====================================================================
//  showHitmap
//=====================================================================
void showHitmapXY() {

  char textfile[MaxPathSize];

  if (SCTOnly) {
    
    cout << " SCT Barrel Hitmaps XY..." << endl; 
   
    sprintf(textfile,"macros/reference/nominal_GlobalPos_SCTBarrel.txt");
    
    cHitmapsXY = new TCanvas("cHitmapsXY","Hitmaps",1100,750);
    cHitmapsXY->Divide(4,3);

    plot(textfile);
    
  }

  return;
}


//=====================================================================
//  plot
//=====================================================================
void plot(char textfile[MaxPathSize]) {

  TNtuple *ntuple = new TNtuple("ntuple","ntuple","mcode:layer:sector:ring:side:x:y:z:axisXx:axisXy");
  ntuple->ReadFile(textfile);
  // ntuple->Scan();

  for (int i=0; i<12; i++) {
    plotRing(ntuple,i);
  } 

  return;
}


//=====================================================================
//  plot
//=====================================================================
void plotRing(TNtuple *ntuple, int icanvas) {

  Int_t SelectedRing = icanvas-6;
  if (SelectedRing>=0) SelectedRing++;

  Float_t iScale = 1.;
  Char_t name[MaxPathSize];

  cHitmapsXY->cd(icanvas+1);
  sprintf(name,"ring==%d",SelectedRing);
  ntuple->Draw("x:y",name,"goff");
  TGraph *hPixelXYRing2a = new TGraph(ntuple->GetSelectedRows(), ntuple->GetV1(), ntuple->GetV2());
  sprintf(name,"XY view (Ring = %d)",SelectedRing);
  hPixelXYRing2a->SetTitle(name);
  hPixelXYRing2a->GetXaxis()->SetTitle("X (mm)");
  hPixelXYRing2a->GetYaxis()->SetTitle("Y (mm)");
  hPixelXYRing2a->GetXaxis()->SetNdivisions(10);
  hPixelXYRing2a->GetYaxis()->SetNdivisions(10);
  hPixelXYRing2a->SetMarkerColor(kWhite);
  hPixelXYRing2a->Draw("AP");
  setTitleFont(font);

  const float PixelWidth = 16.4/2; // mm
  const float SCTWidth = 61.439999/2; // mm

  float mcode, layer, sector, ring, side;
  float x, y, z;
  float axisXx, axisXy;

  ntuple->SetBranchAddress("mcode",&mcode);
  ntuple->SetBranchAddress("layer",&layer);
  ntuple->SetBranchAddress("sector",&sector);
  ntuple->SetBranchAddress("ring",&ring);
  ntuple->SetBranchAddress("side",&side);
  ntuple->SetBranchAddress("x",&x);
  ntuple->SetBranchAddress("y",&y);
  ntuple->SetBranchAddress("z",&z);
  ntuple->SetBranchAddress("axisXx",&axisXx);
  ntuple->SetBranchAddress("axisXy",&axisXy);  

  int entries = ntuple->GetEntries();

  for (int i=0; i<entries; i++) {
    ntuple->GetEntry(i);

    if (ring==SelectedRing) {

      int hits=-999;

      cout << "[" << layer 
	   << ", " << sector
	   << ", " << ring
	   << "]"
	   << endl;

      if (ring>0) hits = GetNumberOfHits(layer,sector+1,ring+6);
      else hits = GetNumberOfHits(layer,sector+1,ring+6+1);

      if (hits>0) {
	
	if (mcode<2000000) {
	  float x1 = (iScale*x)-PixelWidth*(iScale*axisXx);
	  float y1 = (iScale*y)-PixelWidth*(iScale*axisXy);
	  float x2 = (iScale*x)+PixelWidth*(iScale*axisXx);
	  float y2 = (iScale*y)+PixelWidth*(iScale*axisXy);
	}
	else {
	  float x1 = (iScale*x)-SCTWidth*(iScale*axisXx);
	  float y1 = (iScale*y)-SCTWidth*(iScale*axisXy);
	  float x2 = (iScale*x)+SCTWidth*(iScale*axisXx);
	  float y2 = (iScale*y)+SCTWidth*(iScale*axisXy);
	}
	
	
	// cout << " --> (" << x << "," << y << ") :: (" << axisXx << "," << axisXy 
	//      << ") :: (" << x1 << "," << y1 << "," << x2 << "," << y2 << ")" 
	// 	<< endl;
	
	TLine *modLine;
	TLineBorder = new TLine(x1,y1,x2,y2);
	TLineBorder->SetLineColor(kBlack);
	TLineBorder->SetLineWidth(1);
	TLineBorder->Draw();
      }
      
    }
  }

  cHitmapsXY->Update();  
  cHitmapsXY->cd(icanvas+1);
  gPad->SetGridx();
  gPad->SetGridy();
  
  return;
}


//=====================================================================
//  GetNumberOfHits()
//=====================================================================
int GetNumberOfHits(int layer, int sector, int ring) {
  int hits = hSctBa[layer]->GetBinContent(ring,sector);
  return hits;
}
