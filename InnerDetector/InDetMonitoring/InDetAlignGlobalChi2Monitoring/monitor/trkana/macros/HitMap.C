/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

const int NPIX_Layers = 3;
const int NPIX_Discs  = 3;
const int NSCT_Layers = 4;
const int NSCT_Sides  = 2; // el modul del SCT te dos cares
const int NSCT_Discs  = 9;

///////////////////////////////////////////////////////////////////
void HitMap(char nname[80]="../Iter0_trkana.root", int level=0, 
	    const char *chopt="NoText") {
///////////////////////////////////////////////////////////////////
  bool  fent_PIX = 1;
  bool  fent_SCT = 1;
  bool setCommonMax = false;

  //---------------------------------------------------- 
  // carreguem el fitxer
  //----------------------------------------------------
  TFile *f0 = new TFile(nname);
  if(f0->IsZombie()){
    cout << "Malo...:" << nname << " not found !! " << endl;
    return;
  }

  Set_style();
  //---------------------------------------------------- 
  // option handling
  //----------------------------------------------------
  TString *DrawOptions = new TString(chopt);
  DrawOptions->ToLower();
  bool NoText = DrawOptions->Contains("notext");
  NoText ? cout << " --> Printing hit maps (no text) " << endl :  
    cout << " --> Printing hit maps (with entries) " << endl;

  char name[200], title[200];
  int maxx[2];
  int maxb[NSCT_Discs];
  
  ////////////////////
  // PIX            //
  ////////////////////
  if (fent_PIX) {
    TH2F *hPixBa[3];
    TH2F *hPixEc[2][3];

    //------------
    // Barrel
    //------------
    maxx[0]=0;
    for(int layer = 0; layer<NPIX_Layers; layer++){
      sprintf(name,"trkana/PIXHits/hPIXHitMapBaLay%d_%d",layer,level);
      hPixBa[layer] = (TH2F*)f0->Get(name); 
      maxb[layer] = (int) hPixBa[layer]->GetBinContent(hPixBa[layer]->GetMaximumBin());
      maxx[0] = maxb[layer]>maxx[0] ? maxb[layer] : maxx[0];
      SetHistoAtt(hPixBa[layer]);
    }

    TCanvas *CPIXHitMapBa = new TCanvas("CPIXHitMapBa","Pixel Barrel Hitmap",0,450,1000,300); 
    CPIXHitMapBa->Divide(3,1);
    for(int layer=0; layer<NPIX_Layers; layer++){
      CPIXHitMapBa->cd(layer+1);
      if(setCommonMax) hPixBa[layer]->SetMaximum(maxx[0]);  
      NoText ? hPixBa[layer]->Draw("colz") : hPixBa[layer]->Draw("colztext");
    }
    
    //------------
    // Endcaps
    //------------
    for(int ec=0; ec<2; ec++){
      maxx[ec]=0;
      for(int disc=0; disc<NPIX_Discs; disc++){
	sprintf(name,"trkana/PIXHits/hPIXHitMapEndcap%dDisc%d_%d",ec, disc,level);
	hPixEc[ec][disc] = (TH2F*)f0->Get(name); 
	maxb[disc] = (int) hPixEc[ec][disc]->GetBinContent(hPixEc[ec][disc]->GetMaximumBin());
	maxx[ec] = maxb[disc]>maxx[ec] ? maxb[disc] : maxx[ec];
      }
    }

    TCanvas *CPIXHitMapEc = new TCanvas("CPIXHitMapEc","Pixel Endcap HitMap",0,450,1000,600); 
    CPIXHitMapEc->Divide(3,2);

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
      for(int disc=0; disc<NPIX_Discs; disc++){
	CPIXHitMapEc->cd(NPIX_Discs*ec+disc+1);

	// get histograms
	sprintf(name,"trkana/PIXHits/hPIXHitMapEndcap%dDisc%d_%d",ec, disc,level);
	hPixEc[ec][disc] = (TH2F*)f0->Get(name); 

	// create fake histogram just for axes and color palette
	if(!ec) sprintf(name, "EndCap-A PIXEL DISK %d (level %d)",disc, level);
	else sprintf(name, "EndCap-C PIXEL DISK %d (level %d)",disc, level);
	TH2F *box = new TH2F(name, name, 100, -10, 10, 100, -10, 10);
	box->SetStats(kFALSE);
	box->GetXaxis()->SetAxisColor(kWhite);
	box->GetYaxis()->SetAxisColor(kWhite);
	box->GetXaxis()->SetLabelColor(kWhite);
	box->GetYaxis()->SetLabelColor(kWhite);
	box->GetZaxis()->SetLabelSize(0.04);
	for(int bin=1; bin<=hPixEc[ec][disc]->GetNbinsX(); bin++)
	  box->SetBinContent(bin, 1, hPixEc[ec][disc]->GetBinContent(bin));
	if(setCommonMax) box->SetMaximum(maxx[ec]);  

	box->Draw("colz");
	blank->Draw();
	gPad->RedrawAxis();
	draw_pixel_ec_modules(hPixEc[ec][disc]);
	gPad->Update();
      }
    }
   } // end PIX

  ////////////////////
  //     SCT        //
  ////////////////////
  if (fent_SCT) {
    TH2F *hSctBa[4];
    TH2F *hSctEc[2][9];

    //------------
    // Barrel
    //------------
    maxx[0]=0;
    for(int layer = 0; layer<NSCT_Layers; layer++){
      sprintf(name,"trkana/SCTHits/hSCTHitMapBaLay%d_%d",layer,level);
      hSctBa[layer] = (TH2F*)f0->Get(name); 
      maxb[layer] = (int) hSctBa[layer]->GetBinContent(hSctBa[layer]->GetMaximumBin());
      maxx[0] = maxb[layer]>maxx[0] ? maxb[layer] : maxx[0];
      SetHistoAtt(hSctBa[layer]);
    }

    TCanvas *CSCTHitMapBa = new TCanvas("CSCTHitMapBa","Hit Map of SCT Barrel",0,100,1400,300); 
    CSCTHitMapBa->Divide(4,1);    
    for(int layer=0; layer<NSCT_Layers; layer++){
      CSCTHitMapBa->cd(layer+1);
      if(setCommonMax) hSctBa[layer]->SetMaximum(maxx[0]);  
      NoText ? hSctBa[layer]->Draw("colz") : hSctBa[layer]->Draw("colztext");
    }
    
    //------------
    // Endcaps
    //------------
    TCanvas *CSCTHitMapEc[2];

    // loop in end-caps
    for(int ec=0; ec<2; ec++){

      // create canvases per end-cap
      sprintf(name, "CSCTHitMapEc%d",ec);
      if(!ec) sprintf(title, "SCT EndCap A HitMap");
      else sprintf(title, "SCT EndCap C HitMap");
      CSCTHitMapEc[ec] = new TCanvas(name, title, 0,0,1000,1000); 
      CSCTHitMapEc[ec]->Divide(3,3);

      // get histograms and determine maximum
      maxx[ec]=0;
      for(int disc=0; disc<NSCT_Discs; disc++){
	sprintf(name,"trkana/SCTHits/hSCTHitMapEndcap%dDisc%d_%d",ec,disc,level);
	hSctEc[ec][disc] = (TH2F*)f0->Get(name); 
	maxb[disc] = (int) hSctEc[ec][disc]->GetBinContent(hSctEc[ec][disc]->GetMaximumBin());
	maxx[ec] = maxb[disc]>maxx[ec] ? maxb[disc] : maxx[ec];

      }
      // loop in discs
      for(int disc=0; disc<NSCT_Discs; disc++){
	if(!ec) sprintf(name, "EndCap-A SCT DISK %d (level %d)", disc, level);
	else sprintf(name, "EndCap-C SCT DISK %d (level %d)",disc, level);
	cout << name << endl;
	TH2F *boxsct = new TH2F(name, name, 156, -10, 10, 100, -10, 10);
	boxsct->SetStats(kFALSE);
	boxsct->GetXaxis()->SetAxisColor(kWhite);
	boxsct->GetYaxis()->SetAxisColor(kWhite);
	boxsct->GetXaxis()->SetLabelColor(kWhite);
	boxsct->GetYaxis()->SetLabelColor(kWhite);
	boxsct->GetZaxis()->SetLabelSize(0.03);	
	for(int bx=1; bx<=hSctEc[ec][disc]->GetNbinsX(); bx++)      	
	  for(int by=1; by<=hSctEc[ec][disc]->GetNbinsY(); by++)
	    boxsct->SetBinContent(50+bx, 50+by, hSctEc[ec][disc]->GetBinContent(bx,by));
	if(setCommonMax){
	  boxsct->SetMaximum(maxx[ec]);  
	  hSctEc[ec][disc]->SetMaximum(maxx[ec]);
	  
	}
	
	TBox *blanksct = new TBox(-10,-10, 10, 10);
	blanksct->SetFillColor(kWhite);
	
	CSCTHitMapEc[ec]->cd(disc+1);
	boxsct->Draw("colz");
	blanksct->Draw();
	gPad->RedrawAxis();
	char xCommand[80];
	//DrawSCTECModules.C(hSctEc[%d][%d],%d)",ec,disc,disc);
	gROOT->ProcessLine(xCommand);
	draw_sct_ec_modules(hSctEc[ec][disc], disc);
	gPad->Update();
      }      
     }

  } // end SCT

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

///////////////////
void Set_style(){
///////////////////
  gStyle->SetPalette(1);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameLineColor(kWhite);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetCanvasColor(0);
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
