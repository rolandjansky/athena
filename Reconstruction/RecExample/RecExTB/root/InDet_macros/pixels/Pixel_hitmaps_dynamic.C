/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void Pixel_hitmaps_dynamic(int SaveOpt=0) {
  
  // Sytles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("pixel*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);
  
  // build Canvas
  c11 = new TCanvas("c11","Pixel Offline Monitor Hitmaps");
  c11->Divide(LAYERS,MODULES_IN_PHI);

  int i=1;	// module counter

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;
  
  // loop over all modules
  for (phi=MODULES_IN_PHI-1;phi>=0;phi--) {
    for (layer=0;layer<LAYERS;layer++) {
      
      // calc module no.
      module = MODULES_IN_PHI * layer + phi;
      
      // get current subcanvas
      sprintf(name,"c11_%d",i);
      c11->cd(i++);
      TPad* subcanvas = (TPad*)(c11->GetPrimitive(name));
      //subcanvas->SetLogz();
      
      // set name for histo
      sprintf(name,"Hitmap Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"hitmap[%d,%d]",layer,phi);
      //gStyle->SetOptStat(100000);
      
      // get histo for hits
      histo2 = new TH2F(name2,name,144/COL_RESOLUTION,0.,144.,328/ROW_RESOLUTION,0.,328.);
      histo2->SetStats(kFALSE); // statistics in hostogram off
      
      // prepare data from tree for current module (and dont draw it on canvas)
      sprintf(options,"pixel_layer==%d && pixel_phi==%d",layer,phi);
      tree->Draw("pixel_row:pixel_col",options,"goff");
      
      // get data from not yet drawn tree and fill into histo
      for (int ii=0;ii<tree->GetSelectedRows();ii++)
	{
	  treevariable1=*(tree->GetV1()+ii);
	  treevariable2=*(tree->GetV2()+ii);
	  histo2->Fill(treevariable2, treevariable1);
	}
      
      
      //histo2->DrawCopy();
      //histo2->DrawCopy("surf4");
      //histo2->DrawCopy("contz");
      //histo2->DrawCopy("lego");
      
      // now draw histo
      histo2->DrawCopy("colz");
      
      // draw lines in histo for chip borders
      for (int ix=1;ix<CHIPS_PER_MODULE_IN_COL;ix++){
	x[0]=18*ix;
	x[1]=18*ix;
	y[0]=0;
	y[1]=328;
	TGraph *gr1 = new TGraph (2, x, y);
	gr1->SetLineColor(1);
	gr1->Draw();
      }
      
      x[0]=0;
      x[1]=144;
      y[0]=164;
      y[1]=164;
      TGraph *gr1 = new TGraph (2, x, y);
      gr1->SetLineColor(1);
      gr1->Draw();
      
      // update canvas
      c11->Update();
      
      // activate mouse event
      subcanvas->AddExec("dynamic","DynamicExec()");
    }
  }

  sprintf(title, "run %d (%d events)", run, entries);
  c11->cd();
  TPaveText *pt = new TPaveText(0.401, 0.001, 0.6, 0.012);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.01);
  text->SetTextColor(10);
  pt->Draw();
  c11->Update();

  if (SaveOpt==1) c11->Print("Pixels_histograms.ps(");
  else if(SaveOpt==2) { sprintf(sname,"Pixelshitmaps.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}

void DynamicExec()
{
  const int ROW_RESOLUTION=1;		// no of pixels in a row per bin
  const int COL_RESOLUTION=1;		// no of pixels in a col per bin
  
  // get canvas the user's mouse is on
  TObject *select = gPad->GetSelected();
  if(!select) return;
  if (!select->InheritsFrom("TH2")) {gPad->SetUniqueID(0); return;}
  TH2 *h = (TH2*)select;
  gPad->GetCanvas()->FeedbackMode(kTRUE);
  
  //erase old position and draw a line at current position
  int pyold = gPad->GetUniqueID();
  int px = gPad->GetEventX();
  int py = gPad->GetEventY();
  float uxmin = gPad->GetUxmin();
  float uxmax = gPad->GetUxmax();
  int pxmin = gPad->XtoAbsPixel(uxmin);
  int pxmax = gPad->XtoAbsPixel(uxmax);
  if(pyold) gVirtualX->DrawLine(pxmin,pyold,pxmax,pyold);
  gVirtualX->DrawLine(pxmin,py,pxmax,py);
  gPad->SetUniqueID(py);
  Float_t upy = gPad->AbsPixeltoY(py);
  Float_t y = gPad->PadtoY(upy);
  
  //create or set the new canvas c2
  TVirtualPad *padsav = gPad;
  TCanvas *c2 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c2");
  if(c2) delete c2->GetPrimitive("Projection");
  else   c2 = new TCanvas("c2","Projection Canvas",710,10,700,500);
  c2->SetGrid();
  c2->cd();
  
  // find the bin the mouse is on
  Int_t biny = h->GetYaxis()->FindBin(y);
  // draw projection
  TH1D *hp = h->ProjectionX("",biny,biny);
  hp->SetFillColor(38);
  char title[80];
  
  sprintf(title,"Column %d from %s",biny*COL_RESOLUTION,h->GetTitle());
  hp->SetName("Projection");
  hp->SetTitle(title);
  hp->Draw();
  c2->Update();
  padsav->cd();
}
