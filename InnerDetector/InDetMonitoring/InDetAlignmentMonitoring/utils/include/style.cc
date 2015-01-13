/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TROOT.h>
#include <TStyle.h>
#include <TGaxis.h>
#include <TPaveText.h>
#include <TPaveStats.h>

const Int_t NContLevels = 50;
Int_t RM_ColorIndex[NContLevels];
float RM_Levels[NContLevels];
const int iNPIX_Layers = 3;
const int iNPIX_Discs = 3;
const int iNSCT_Layers = 4;
const int iNSCT_Discs = 9;

void setStyle() {
	if (AtlasStyle) {
		gROOT->SetStyle("ATLAS");
		ColorForSecond = kRed;
		ColorForFirst	= kGray;
		// Extra options to the ATLAS Style
		Float_t jsize = 0.04;
	
		gStyle->SetLabelSize(jsize,"x");
		gStyle->SetLabelSize(jsize,"y");
		gStyle->SetLabelSize(jsize-0.01,"z");
	
		gStyle->SetTitleSize(jsize,"x");	
		gStyle->SetTitleSize(jsize,"y");
		gStyle->SetTitleSize(jsize,"z");
		gStyle->SetTextFont(42);
	
		gStyle->SetStripDecimals(false);		
		TGaxis::SetMaxDigits(4);
		gStyle->SetPalette(1);
	}
	else{
	 // Default options
	 	ColorForSecond = kRed;
		ColorForFirst	= kGray;
		Float_t isize = 0.04;
		font = 42;

		gStyle->SetOptStat(111110);

		gStyle->SetCanvasColor(kWhite);	
		gStyle->SetCanvasBorderMode(0);
		gStyle->SetCanvasBorderSize(0);

		gStyle->SetPadBorderMode(0);

		gStyle->SetLabelFont(font,"X");
		gStyle->SetLabelFont(font,"Y");
		gStyle->SetLabelFont(font,"Z");
		gStyle->SetLabelSize(isize,"x");
		gStyle->SetLabelSize(isize,"y");
		gStyle->SetLabelSize(isize-0.01,"z");

		gStyle->SetTitleFont(font,"x");
		gStyle->SetTitleFont(font,"y");
		gStyle->SetTitleFont(font,"z");
		gStyle->SetTitleSize(isize,"x");	
		gStyle->SetTitleSize(isize,"y");
		gStyle->SetTitleSize(isize,"z");
		gStyle->SetTitleYOffset(1.5);

		gStyle->SetOptFit(0);

		gStyle->SetStatW(0.3);
		//	gStyle->SetStatH(0.15);
		gStyle->SetStatColor(kWhite);	
		gStyle->SetStatFont(font);
		gStyle->SetStatFontSize(isize);
		// gStyle->SetStatStyle(0); // to view data behind box too!
		// gStyle->SetStatBorderSize(0);	 // remove shade from stat box
	
		gStyle->SetStripDecimals(false);
		TGaxis::SetMaxDigits(4);

		gStyle->SetTextFont(font);

		gStyle->SetFrameBorderMode(0);

		gStyle->SetPalette(1);

		gStyle->SetPaintTextFormat("g");

		gStyle->SetPadLeftMargin(0.12);
		gStyle->SetPadRightMargin(0.10);
		gStyle->SetPadTopMargin(0.12);
		// gStyle->SetPadBottomMargin(0.15);
	}
	
	gROOT->ForceStyle();
}


void ScalePlots(Int_t position) {
	if (normalizePlots) {
		TLatex *t2 = new TLatex();
		Char_t name[150];
		t2->SetNDC();
		t2->SetTextFont(font);
		t2->SetTextSize(0.08);
		t2->SetTextAlign(12);
		t2->SetTextSize(0.035);
		t2->SetTextColor(kRed);
		sprintf(name,"Normalized plots");
		if(!AtlasStyle){
			if (position==0) {
				t2->SetTextAngle(270);
				t2->DrawLatex(0.93,0.37,name);
			}
			else
				t2->DrawLatex(0.3,0.91,name);
		}
		else {
			if (position==0) {
				t2->SetTextAngle(270);
				t2->DrawLatex(0.97,0.47,name);
			}
			else
				t2->DrawLatex(0.3,0.91,name);
		}
	}
}

//=====================================================================
//  setTitleFont()
//=====================================================================
void setTitleFont(Int_t font) {
  if (!AtlasStyle) {
    gPad->Update();
    TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
    pt->SetTextFont(font);
    pt->SetTextSize(0.04);
    gPad->Modified();
  }
}

void setTitleSize(double x2Size) {
	if (!AtlasStyle) {
		gPad->Update();
		TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
		pt->SetX2NDC(x2Size);
		gPad->Modified();
	}
}

//=====================================================================
//  ForceStyle()
//=====================================================================
void ForceStyle(TH1F* histoIn1D) {
	histoIn1D->GetXaxis()->SetLabelFont(42);
	histoIn1D->GetXaxis()->SetTitleFont(42);
	histoIn1D->GetYaxis()->SetLabelFont(42);
	histoIn1D->GetYaxis()->SetTitleFont(42);
	histoIn1D->GetYaxis()->SetTitleOffset(1.5);
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

void SetHistoAtt(TH2 *histo){
  ///////////////////////////// 
  histo->SetStats(kFALSE);  
  histo->GetXaxis()->SetLabelSize(0.03);
  histo->GetXaxis()->SetTitleSize(0.04);
  histo->GetYaxis()->SetLabelSize(0.03);
  histo->GetYaxis()->SetTitleSize(0.04);
  histo->GetZaxis()->SetLabelSize(0.03);
}

//=====================================================================
//  moveTPaveStats()
//=====================================================================
void moveTPaveStats(TH2F* histoIn2D) {
  if (!AtlasStyle) {
    TPaveStats *st1 = (TPaveStats*)histoIn2D->FindObject("stats"); 
    st1->SetX1NDC(0.56); //new x start position
    st1->SetX2NDC(0.89); //new x start position
    st1->SetY1NDC(0.80); //new y end position
    st1->SetOptStat(1110);
    st1->Draw();
  }
}



/////////////////////////////////////////////////////////////////////
int locateColor(const int ncols, float min, float max, float cont){
  /////////////////////////////////////////////////////////////////////
  float val[100];
  float step = (max-min)/ncols;

  val[0] = min + step/2;
  for(int i=1; i<ncols; i++)
    val[i] = val[i-1] + step;

  if(cont<=val[0])
    return 0;
  else if(cont >= val[ncols-1])
    return ncols-1;
  else{
    for(int i=1; i<ncols; i++){
      if(cont>=val[i-1] && cont<val[i]){
		return i;
      }    
    }
  }
  return 0;
}

void SetZAxisTitle(TH2F *histo2D,
	                 Float_t X1, Float_t X2,
	                 Float_t Y1, Float_t Y2) {
	
	// moving the color scale
	gPad->Update();
	TPaletteAxis *palette = (TPaletteAxis*)histo2D->GetListOfFunctions()->FindObject("palette");
	palette->SetX1NDC(X1);
	palette->SetX2NDC(X2);
	palette->SetY1NDC(Y1);
	palette->SetY2NDC(Y2);
	gPad->Modified();
	
	TLatex *t = new TLatex();
	t->SetNDC();
	t->SetTextFont(font);
	t->SetTextSize(0.041);
	t->DrawLatex(X1-0.01,0.90,"res (#mum)");
	
	return;
}

void draw_pixel_ec_modules(TH1F *histo){
  //////////////////////////////////////////////////////
  const int npixmods=48;  
  const float min = histo->GetMinimum();
  const float max = histo->GetMaximum();

  TPolyLine *p[npixmods];
  double x[5], y[5];

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
				histo->GetBinContent(i+1));
    p[i] = new TPolyLine(5,x,y);
    p[i]->SetFillColor(gStyle->GetColorPalette(col_index));
  }

  for(int i=0; i<npixmods; i++)
    p[i]->Draw("Fsame");
  setTitleFont(font);

}

void SCTMaps_SetXaxisLabels(TH2F *histoMap)
{
  for (Int_t bin=1; bin<= histoMap->GetNbinsX(); bin++) {
    if (bin== 1) histoMap->GetXaxis()->SetBinLabel(bin,"-6");   
    if (bin== 2) histoMap->GetXaxis()->SetBinLabel(bin,"-5");   
    if (bin== 3) histoMap->GetXaxis()->SetBinLabel(bin,"-4");   
    if (bin== 4) histoMap->GetXaxis()->SetBinLabel(bin,"-3");   
    if (bin== 5) histoMap->GetXaxis()->SetBinLabel(bin,"-2");   
    if (bin== 6) histoMap->GetXaxis()->SetBinLabel(bin,"-1");   
    if (bin== 7) histoMap->GetXaxis()->SetBinLabel(bin,"1");   
    if (bin== 8) histoMap->GetXaxis()->SetBinLabel(bin,"2");   
    if (bin== 9) histoMap->GetXaxis()->SetBinLabel(bin,"3");   
    if (bin==10) histoMap->GetXaxis()->SetBinLabel(bin,"4");   
    if (bin==11) histoMap->GetXaxis()->SetBinLabel(bin,"5");   
    if (bin==12) histoMap->GetXaxis()->SetBinLabel(bin,"6");   
  }
  return;
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
     
      if (false) {
	cout << " -- draw_sct_ec_modules -- disk: " << disc << "  ring: " << ring << "  module: " << mod << "   Entries: " <<  histo->GetBinContent(ring+1,mod+1) << endl;
      }

    } // end loop in modules

    for(int mod=0; mod<nmods[ring]; mod++)
      p[mod]->Draw("Fsame");
      
  } // end loop in rings

  return;
}


