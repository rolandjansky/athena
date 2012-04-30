/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void Pixel_hitmaps_tot_dynamic(int SaveOpt=0) {

  // Styles
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
  c11 = new TCanvas("c11","Pixel Offline Monitor");
  c11->Divide(LAYERS, MODULES_IN_PHI);
  
  int i=1;  // counter for modules

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
      sprintf(name,"ToT-Map Module %d: Layer %d, Phi %d",module,layer,phi);
      sprintf(name2,"map_tot[%d,%d]",layer,phi);
      //gStyle->SetOptStat(100000);
      
      // get histo for tot-map
      TH2F *histo = new TH2F(name2,name,144/COL_RESOLUTION,0.,144.,328/ROW_RESOLUTION,0.,328.);
      histo->SetStats(kFALSE);

      // get histo for ToT-distribution
      sprintf(name2,"tot[%d,%d]",layer,phi);
      TH3F *tothist = new TH3F(name2,"",144/COL_RESOLUTION,0.,144.,328/ROW_RESOLUTION,0.,328.,40,0,80);

      // prepare data from tree for current module (and dont draw it on canvas)
      sprintf(options,"pixel_layer==%d && pixel_phi==%d",layer,phi);
      tree->Draw("pixel_row:pixel_col:pixel_tot",options,"goff");
      
      // get data from not yet drawn tree and fill into histos        
      for (int ii=0;ii<tree->GetSelectedRows();ii++)
	{
          treevariable1=*(tree->GetV1()+ii);
	  treevariable2=*(tree->GetV2()+ii);
	  treevariable3=*(tree->GetV3()+ii);
	  histo->Fill(treevariable2, treevariable1, treevariable3);
	  tothist->Fill(treevariable2, treevariable1, treevariable3);
	}
      
      histo->SetStats(kFALSE); // statistics in histogram off
      
      //histo->DrawCopy();
      //histo->DrawCopy("surf4");
      //histo->DrawCopy("contz");
      // now draw histo (tot-map)
      histo->DrawCopy("colz");
      
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
      // avoid problems with mouse over border lines
      subcanvas->SetEditable(kFALSE);
    }
  }  


  sprintf(title, "run %d (%d events)", run, entries);
  c11->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c11->Update();
    
  if (SaveOpt==1) c11->Print("Pixels_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"Pixels_tot_hitmaps.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}

void DynamicExec()
{
  const int ROW_RESOLUTION=1;
  const int COL_RESOLUTION=1;     
  
  // get canvas the user's mouse is on
  TObject *select = gPad->GetSelected();
  if(!select) return;
  if (!select->InheritsFrom("TH2")) {gPad->SetUniqueID(0); return;}
  TH2 *h = (TH2*)select;
  gPad->GetCanvas()->FeedbackMode(kTRUE);
  gPad->SetEditable(kTRUE);
  
  // get position
  int pyold = gPad->GetUniqueID();
  int px = gPad->GetEventX();
  int py = gPad->GetEventY();
  Float_t upy = gPad->AbsPixeltoY(py);
  Float_t y = gPad->PadtoY(upy);
  Float_t upx = gPad->AbsPixeltoX(px);
  Float_t x = gPad->PadtoX(upx);
  
  // draw rectangle around selected pixels
  double xn[5], yn[5];
  xn[0]=x;
  yn[0]=y;
  xn[1]=x+10*COL_RESOLUTION;
  yn[1]=y;
  xn[2]=x+10*COL_RESOLUTION;
  yn[2]=y+10*ROW_RESOLUTION;
  xn[3]=x;
  yn[3]=y+10*ROW_RESOLUTION;
  xn[4]=x;
  yn[4]=y;
  TGraph *gr1 = (TGraph*)gPad->GetListOfPrimitives()->FindObject("marker");
  //TGraph *gr1 = (TGraph*)gPad->GetPrimitive("marker");
  if(gr1) delete gPad->GetPrimitive("marker");
  //else   
  gr1 = new TGraph(5, xn, yn);
  //TGraph *gr1 = new TGraph (5, xn, yn);
  gr1->SetLineColor(2);
  gr1->SetName("marker");
  gr1->Draw();
  gPad->Update();
  gPad->SetEditable(kFALSE);
  
  // get the right tot-distribution histo
  TH3F *histo = (TH3F*)gDirectory->GetList()->FindObject((h->GetName())+4);
  //create or set the new canvas c2
  TVirtualPad *padsav = gPad;
  TCanvas *c2 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c2");
  if(c2) delete c2->GetPrimitive("Projection");
  else   {c2 = new TCanvas("c2","Projection Canvas",710,10,700,800);
  c2->Divide(1,2);}
  c2->SetGrid();
  
  
  // get bins
  Int_t biny = h->GetYaxis()->FindBin(y);
  Int_t binx = h->GetXaxis()->FindBin(x);
  
  
  c2->cd(1);
  //cout << "project:" << binx<< ", " << biny << endl;
  // make histo of ToT-distribution for selected pixels
  TH1D *hp1 = histo->ProjectionZ("ToT",binx,binx+10,biny,biny+10);
  hp1->SetFillColor(38);
  char title[90];
  sprintf(title,"ToT distribution for %s, col %d to %d and row %d to %d",h->GetTitle()+8,x,x+10*COL_RESOLUTION,y,y+10*ROW_RESOLUTION);
  //hp1->SetName("ToT");
  hp1->SetTitle(title);
  hp1->Draw("B");
  
  c2->cd(2);
  //TH2F *hitmaplarge = (TH2F*)h->Clone();
  //hitmaplarge->SetName("hitmaplarge");
  //char cut[90];
  //sprintf(cut,"(x>=%d)&&(x<=%d)&&(y>=%d)&&(y<=%d)",x,x+2*COL_RESOLUTION,y,y+2*ROW_RESOLUTION);
  /*TCutG *cutg = new TCutG("cutg",5);
    cutg->SetPoint(0,x,y);
    cutg->SetPoint(1, x+2*COL_RESOLUTION,y);
    cutg->SetPoint(2, x+2*COL_RESOLUTION, y+2*ROW_RESOLUTION);
    cutg->SetPoint(3,x, y+2*ROW_RESOLUTION);
    cutg->SetPoint(4,x,y);
    hitmaplarge->Draw("contz [cutg]");*/
  
  // update the canvas
  c2->Update();
  padsav->cd();
}
