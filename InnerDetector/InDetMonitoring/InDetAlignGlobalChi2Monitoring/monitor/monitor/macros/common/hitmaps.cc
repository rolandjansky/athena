/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hitmaps(const char *chopt="Text") {

  const int iNPIX_Layers = 3;
  const int iNPIX_Discs  = 3;
  const int iNSCT_Layers = 4;
  const int iNSCT_Sides  = 2; // SCT modules has two sides
  const int iNSCT_Discs  = 9;
  
  bool setCommonMax = true;

  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetPadTopMargin(0.12);
  gROOT->ForceStyle();

  //---------------------------------------------------- 
  // option handling
  //----------------------------------------------------
  TString *DrawOptions = new TString(chopt);
  DrawOptions->ToLower();
  bool NoText = DrawOptions->Contains("notext");
  NoText ? cout << " Hitmaps..." << endl : cout << " Hitmaps (with entries)..." << endl;
  
  char name[200], title[200];
  int maxx[2];
  int maxb[iNSCT_Discs];
  
  ////////////////////
  // PIX            //
  ////////////////////
  if (!SCTOnly && iPixelHits>0) {
    
    TH2F *hPixBa[3];
    TH2F *hPixEc[2][3];

    cPIXHitMap = new TCanvas("cPIXHitMap","Pixel Barrel Hitmap",
			     CanvasSizeX9[0], CanvasSizeX9[1],
			     CanvasSizeX9[2], CanvasSizeX9[3]);
    cPIXHitMap->Divide(3,3);

    //------------
    // Barrel
    //------------
    cout << "   - Pixel Barrel..." << endl;

    maxx[0]=0;
    for(int layer = 0; layer<iNPIX_Layers; layer++){
      sprintf(name,"trkana/PIXHits/hPIXHitMapBaLay%d_%d",layer,level);
      hPixBa[layer] = (TH2F*)file->Get(name); 
      maxb[layer] = (int) hPixBa[layer]->GetBinContent(hPixBa[layer]->GetMaximumBin());
      maxx[0] = maxb[layer]>maxx[0] ? maxb[layer] : maxx[0];
      SetHistoAtt(hPixBa[layer]);
    }

    for(int layer=0; layer<iNPIX_Layers; layer++){
      cPIXHitMap->cd(layer+1+3);
      if(setCommonMax) hPixBa[layer]->SetMaximum(maxx[0]);
      sprintf(name,"Hitmap - Pixel Barrel Layer %d",layer);
      hPixBa[layer]->SetTitle(name);
      hPixBa[layer]->GetYaxis()->SetTitleOffset(1.0);
      NoText ? hPixBa[layer]->Draw("colz") : hPixBa[layer]->Draw("colztext");
      gPad->Update();
      setTitleFont(font);
    }

    //------------
    // Endcaps
    //------------
    cout << "   - Pixel EndCaps..." << endl;
    for(int ec=0; ec<2; ec++){
      maxx[ec]=0;
      for(int disc=0; disc<iNPIX_Discs; disc++){
	sprintf(name,"trkana/PIXHits/hPIXHitMapEndcap%dDisc%d_%d",ec, disc,level);
	hPixEc[ec][disc] = (TH2F*)file->Get(name); 
	maxb[disc] = (int) hPixEc[ec][disc]->GetBinContent(hPixEc[ec][disc]->GetMaximumBin());
	maxx[ec] = maxb[disc]>maxx[ec] ? maxb[disc] : maxx[ec];
      }
    }

    /* Box filled in white to hide histogram. Could not get rif off this since 
       a TH2F needs to be drawn afterwards with chopt_z option for the 
       color palette to be created. Tried to draw in a pad and then 
       superimpose the "box" histo but then any pointer to the original 
       TPaletteAxis gets of course deleted. Changing the range in x-axis to make 
       "disappear" the visible entries ends with the same feature.
    */

    TBox *blank = new TBox(-10,-10, 10,-8);
    blank->SetFillColor(kWhite);

    for(int ec=0; ec<2; ec++){
      for(int disc=0; disc<iNPIX_Discs; disc++){
	if(!ec) cPIXHitMap->cd(iNPIX_Discs*ec+disc+1);
	else cPIXHitMap->cd(iNPIX_Discs*ec+disc+1+3);

	// get histograms
	sprintf(name,"trkana/PIXHits/hPIXHitMapEndcap%dDisc%d_%d",ec, disc,level);
	hPixEc[ec][disc] = (TH2F*)file->Get(name); 

	// create fake histogram just for axes and color palette
	if(!ec) sprintf(name, "Hitmap - Pixel EndCapA Disk %d",disc);
	else sprintf(name, "Hitmap - Pixel EndCapC Disk %d",disc);
	TH2F *box = new TH2F(name, name, 100, -10, 10, 100, -10, 10);
	box->SetStats(kFALSE);
	for(int bin=1; bin<=hPixEc[ec][disc]->GetNbinsX(); bin++)
	  box->SetBinContent(bin, 1, hPixEc[ec][disc]->GetBinContent(bin));
	if(setCommonMax) box->SetMaximum(maxx[ec]);  
	box->GetXaxis()->SetLabelColor(kWhite);
	box->GetYaxis()->SetLabelColor(kWhite);
	box->GetYaxis()->SetTitle("Y axis");
	box->GetXaxis()->SetTitle("X axis");
	box->GetYaxis()->SetTitleOffset(0.8);
	box->Draw("colz");
	blank->Draw();
	gPad->RedrawAxis();
	draw_pixel_ec_modules(hPixEc[ec][disc]);
	setTitleFont(font);
	gPad->Update();
      }
    }
  } // end PIX

  ////////////////////
  //     SCT        //
  ////////////////////
  if (!PixelOnly && iSCTHits>0) {
    
    TH2F *hSctBa[4];
    TH2F *hSctEc[2][9];

    //------------
    // Barrel
    //------------
    cout << "   - SCT Barrel..." << endl;

    // Module physical axis in Z (to be drawn on top along graphics X)
    TGaxis *ATLZaxis[4]; 
    TGaxis *ATLPhiaxis[4]; 

    maxx[0]=0;
    for(int layer = 0; layer<iNSCT_Layers; layer++){
      sprintf(name,"trkana/SCTHits/hSCTHitMapBaLay%d_%d",layer,level);
      hSctBa[layer] = (TH2F*)file->Get(name); 
      maxb[layer] = (int) hSctBa[layer]->GetBinContent(hSctBa[layer]->GetMaximumBin());
      maxx[0] = maxb[layer]>maxx[0] ? maxb[layer] : maxx[0];
      SetHistoAtt(hSctBa[layer]);
      sprintf(name,"Hitmap - SCT Barrel Layer %d",layer);
      hSctBa[layer]->SetTitle(name);
      hSctBa[layer]->GetYaxis()->SetTitleOffset(1.0);
      hSctBa[layer]->GetXaxis()->SetLabelSize(0.04);
      hSctBa[layer]->SetXTitle("ring");
      hSctBa[layer]->SetYTitle("sector");
    }

    cSCTHitMapBa = new TCanvas("cSCTHitMapBa","Hit Map of SCT Barrel",CanvasSizeX4[0],CanvasSizeX4[1]); 
    cSCTHitMapBa->Divide(2,2);
    for(int layer=0; layer<iNSCT_Layers; layer++){
      cSCTHitMapBa->cd(layer+1);
      if(setCommonMax) hSctBa[layer]->SetMaximum(maxx[0]);  
      NoText ? hSctBa[layer]->Draw("colz") : hSctBa[layer]->Draw("colztext");
      setTitleFont(font);
      // Z axis on top (only for histos with 12 bins) 
      if (hSctBa[layer]->GetNbinsX()==12) {
	ATLZaxis[layer] = new TGaxis(-5.5,31.5,6.5,31.5,-800,800,510,"-");
	ATLZaxis[layer]->SetName("Z");
	if (layer == 0) {ATLZaxis[layer]->SetY1(31.5);ATLZaxis[layer]->SetY2(31.5);}
	if (layer == 1) {ATLZaxis[layer]->SetY1(39.5);ATLZaxis[layer]->SetY2(39.5);}
	if (layer == 2) {ATLZaxis[layer]->SetY1(47.5);ATLZaxis[layer]->SetY2(47.5);}
	if (layer == 3) {ATLZaxis[layer]->SetY1(55.5);ATLZaxis[layer]->SetY2(55.5);}
	ATLZaxis[layer]->SetLabelSize(hSctBa[layer]->GetYaxis()->GetLabelSize());
	ATLZaxis[layer]->SetLabelOffset(0.0);
	ATLZaxis[layer]->SetLabelFont(hSctBa[layer]->GetXaxis()->GetLabelFont());
	ATLZaxis[layer]->SetTitleSize(hSctBa[layer]->GetXaxis()->GetTitleSize());
	ATLZaxis[layer]->SetTitleFont(hSctBa[layer]->GetXaxis()->GetTitleFont());
	ATLZaxis[layer]->SetTitle(" z (mm)");
	ATLZaxis[layer]->Draw();
      }
      // moving the color scale
      gPad->Update();
      TPaletteAxis *palette = 
	(TPaletteAxis*)hSctBa[layer]->GetListOfFunctions()->FindObject("palette");
      palette->SetX1NDC(0.905);
      palette->SetX2NDC(0.935);
      palette->SetY1NDC(0.1);
      palette->SetY2NDC(0.75);
      gPad->Modified();
      // Phi axis on the right 
      if (1==1) {
	ATLPhiaxis[layer] = new TGaxis(6.5,-0.5,6.5,31.5,0.,360.,510,"+L");
	ATLPhiaxis[layer]->SetName("Z");
	if (layer == 0) {ATLPhiaxis[layer]->SetY2(31.5);}
	if (layer == 1) {ATLPhiaxis[layer]->SetY2(39.5);}
	if (layer == 2) {ATLPhiaxis[layer]->SetY2(47.5);}
	if (layer == 3) {ATLPhiaxis[layer]->SetY2(55.5);}
	ATLPhiaxis[layer]->SetLabelSize(hSctBa[layer]->GetYaxis()->GetLabelSize());
	ATLPhiaxis[layer]->SetLabelOffset(0.0);
	ATLPhiaxis[layer]->SetLabelFont(hSctBa[layer]->GetXaxis()->GetLabelFont());
	ATLPhiaxis[layer]->SetTitleSize(hSctBa[layer]->GetXaxis()->GetTitleSize());
	ATLPhiaxis[layer]->SetTitleFont(hSctBa[layer]->GetXaxis()->GetTitleFont());
	ATLPhiaxis[layer]->SetTitle("#phi (deg)");
	ATLPhiaxis[layer]->SetLabelOffset(0.01);
	ATLPhiaxis[layer]->Draw();
      }
    }

    if (!SCTBarrelOnly) {

      //------------
      // Endcaps
      //------------
      cout << "   - SCT EndCaps..." << endl;

      // loop in end-caps
      for(int ec=0; ec<2; ec++){

	// create canvases per end-cap
	sprintf(name, "cSCTHitMapEc%d",ec);
	if(!ec) sprintf(title, "SCT EndCap A HitMap");
	else sprintf(title, "SCT EndCap C HitMap");
	cSCTHitMapEc[ec] = new TCanvas(name, title,
				       CanvasSizeX9[0], CanvasSizeX9[1],
				       CanvasSizeX9[2], CanvasSizeX9[3]);
	cSCTHitMapEc[ec]->Divide(3,3);

	// get histograms and determine maximum
	maxx[ec]=0;
	for(int disc=0; disc<iNSCT_Discs; disc++){
	  sprintf(name,"trkana/SCTHits/hSCTHitMapEndcap%dDisc%d_%d",ec,disc,level);
	  hSctEc[ec][disc] = (TH2F*)file->Get(name); 
	  maxb[disc] = (int) hSctEc[ec][disc]->GetBinContent(hSctEc[ec][disc]->GetMaximumBin());
	  maxx[ec] = maxb[disc]>maxx[ec] ? maxb[disc] : maxx[ec];
	}
         
	// loop in discs
	for(int disc=0; disc<iNSCT_Discs; disc++){
	  if(!ec) sprintf(name, "Hitmap - SCT EndCapA Disk %d", disc);
	  else sprintf(name, "Hitmap - SCT EndCapC Disk %d",disc);
	  TH2F *boxsct = new TH2F(name, name, 156, -10, 10, 100, -10, 10);
	  boxsct->SetStats(kFALSE);
	  for(int bx=1; bx<=hSctEc[ec][disc]->GetNbinsX(); bx++)      	
	    for(int by=1; by<=hSctEc[ec][disc]->GetNbinsY(); by++)
	      boxsct->SetBinContent(50+bx, 50+by, hSctEc[ec][disc]->GetBinContent(bx,by));
	  if(setCommonMax){
	    boxsct->SetMaximum(maxx[ec]);
	    hSctEc[ec][disc]->SetMaximum(maxx[ec]);
	  }
	
	  TBox *blanksct = new TBox(-10,-10, 10, 9.85);
	  blanksct->SetFillColor(kWhite);
	
	  cSCTHitMapEc[ec]->cd(disc+1);
	  boxsct->GetXaxis()->SetLabelColor(kWhite);
	  boxsct->GetYaxis()->SetLabelColor(kWhite);
	  boxsct->GetYaxis()->SetTitle("Y axis");
	  boxsct->GetXaxis()->SetTitle("X axis");
	  boxsct->GetYaxis()->SetTitleOffset(0.8);
	  boxsct->Draw("colz");
	  blanksct->Draw();
	  gPad->RedrawAxis();
	  draw_sct_ec_modules(hSctEc[ec][disc], disc);
	  setTitleFont(font);
	  gPad->Update();
	}      
      }
    }

  } // end SCT

  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadTopMargin(0.12);
  gROOT->ForceStyle();

  return;
}

//////////////////////////////////////////////////////
void draw_pixel_ec_modules(TH2F *histo){
  //////////////////////////////////////////////////////
  const int npixmods=48;  
  const float min = histo->GetMinimum();
  const float max = histo->GetMaximum();

  TPolyLine *p[npixmods];
  double x[5], y[5];
  double u[5], v[5];

  const float hxl = 1;
  const float hxh = 3;
  const float hy  = 4;
  const float phistep = 2*TMath::Pi()/npixmods;
  const float inr =  5; 
  const float our = 10; 

  for(int i=0; i<npixmods; i++){
    for(int j=0; j<5; j++){
      x[j]=0;
      y[j]=0;
    }

    float phi = -i*phistep;

    x[0] = inr*cos(phi-phistep/2);
    x[1] = inr*cos(phi+phistep/2);
    x[2] = our*cos(phi+phistep/2);
    x[3] = our*cos(phi-phistep/2);
    x[4] = inr*cos(phi-phistep/2);

    y[0] = inr*sin(phi-phistep/2);
    y[1] = inr*sin(phi+phistep/2);
    y[2] = our*sin(phi+phistep/2);
    y[3] = our*sin(phi-phistep/2);
    y[4] = inr*sin(phi-phistep/2);

    int col_index = locateColor(gStyle->GetNumberOfColors(), 
				min, max, 
				histo->GetBinContent(1,i+1));
    p[i] = new TPolyLine(5,x,y);
    p[i]->SetFillColor(gStyle->GetColorPalette(col_index));
  }

  for(int i=0; i<npixmods; i++)
    p[i]->Draw("Fsame");
  setTitleFont(font);

}


/////////////////////////////////////////////////////////////////////
int locateColor(const int ncols, float min, float max, float cont){
  /////////////////////////////////////////////////////////////////////
  float val[100];
  float step = (max-min)/ncols;
  int res;

  val[0] = min + step/2;
  for(int i=1; i<ncols; i++)
    val[i] = val[i-1] + step;

  if(cont<=val[0])
    res = 0;
  else if(cont>=val[ncols-1])
    res = ncols-1;
  else{
    for(int i=1; i<ncols; i++){
      if(cont>=val[i-1] && cont<val[i]){
	res = i;
	break;
      }    
    }
  }
  return res;
}

void draw_sct_ec_modules(TH2F *histo, int disc) {
//////////////////////////////////////////////////////
  /*  Number of rings (i.e, module type) in each disk. 
      Values mean:
      1 : outers
      2 : outers + middles
      3 : outers + middles + inners       
  */

  const int nrings[9]={2,3,3,3,3,3,2,2,1};
  
  // number of modules in each ring
  const int nmods[3]={52,40,40};

  float inner_radius[3]={ 7.5, 4.9, 3.4};
  float outer_radius[3]={10.0, 7.4, 4.8};
  // the one before the last wheel has short middle modules
  if (disc==7) {
    inner_radius[1]=6.0;
  }

  TPolyLine *p[52];
  double x[5], y[5];
  double u[5], v[5];

  const float hxl = 1;
  const float hxh = 3;
  const float hy  = 4;

  const float min = histo->GetMinimum();
  const float max = histo->GetMaximum();

  // loop in rings within disk
  for(int ring=0; ring<nrings[disc]; ring++){    
    
    const float phistep = 2*TMath::Pi()/nmods[ring];
    const float inr = inner_radius[ring]; 
    const float our = outer_radius[ring]; 

    // loop in modules within ring
    for(int mod=0; mod<nmods[ring]; mod++){

      for(int j=0; j<5; j++){
	x[j]=0;
	y[j]=0;
      }

      // clockwise if looking z-axis into the page
      float phi = -mod*phistep; 
      
      x[0] = inr*cos(phi-phistep/2);
      x[1] = inr*cos(phi+phistep/2);
      x[2] = our*cos(phi+phistep/2);
      x[3] = our*cos(phi-phistep/2);
      x[4] = inr*cos(phi-phistep/2);
      
      y[0] = inr*sin(phi-phistep/2);
      y[1] = inr*sin(phi+phistep/2);
      y[2] = our*sin(phi+phistep/2);
      y[3] = our*sin(phi-phistep/2);
      y[4] = inr*sin(phi-phistep/2);

      int col_index = locateColor(gStyle->GetNumberOfColors(), 
      				  min, max, 
				  histo->GetBinContent(ring+1,mod+1));
      p[mod] = new TPolyLine(5,x,y);
      if( histo->GetBinContent(ring+1,mod+1)> -1000000) p[mod]->SetFillColor(gStyle->GetColorPalette(col_index));
      else  p[mod]->SetFillColor(kBlack);
     
      

    } // end loop in modules

    for(int mod=0; mod<nmods[ring]; mod++)
      p[mod]->Draw("Fsame");
      
  } // end loop in rings

  return;
}

/////////////////////////////
void SetHistoAtt(TH1 *histo){
  ///////////////////////////// 
  histo->SetStats(kFALSE);  
  histo->GetXaxis()->SetLabelSize(0.03);
  histo->GetXaxis()->SetTitleSize(0.04);
  histo->GetYaxis()->SetLabelSize(0.03);
  histo->GetYaxis()->SetTitleSize(0.04);
  histo->GetZaxis()->SetLabelSize(0.03);
}
