/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>
#include <cmath>

#include <TStyle.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TROOT.h>

#define def_Preliminary false
#define def_Official false

inline void SetAtlasStyle(){
	TStyle *atlasStyle= new TStyle("ATLAS","Atlas style");

	// use plain black on white colors
	Int_t icol=0;
	atlasStyle->SetFrameBorderMode(icol);
	atlasStyle->SetCanvasBorderMode(icol);
  	atlasStyle->SetFrameFillColor(icol);
	atlasStyle->SetPadBorderMode(icol);
	atlasStyle->SetPadColor(icol);
	atlasStyle->SetCanvasColor(icol);
	atlasStyle->SetStatColor(icol);
	//atlasStyle->SetFillColor(icol);

	// set the paper & margin sizes
	atlasStyle->SetPaperSize(20,26);
	atlasStyle->SetPadTopMargin(0.05);
	atlasStyle->SetPadRightMargin(0.05);
	atlasStyle->SetPadBottomMargin(0.16);
	atlasStyle->SetPadLeftMargin(0.12);

	// use large fonts
	//Int_t font=72;
	Int_t font=42;
	Double_t tsize=0.05;
	atlasStyle->SetTextFont(font);


	atlasStyle->SetTextSize(tsize);
	atlasStyle->SetLabelFont(font,"x");
	atlasStyle->SetTitleFont(font,"x");
	atlasStyle->SetLabelFont(font,"y");
	atlasStyle->SetTitleFont(font,"y");
	atlasStyle->SetLabelFont(font,"z");
	atlasStyle->SetTitleFont(font,"z");

	atlasStyle->SetLabelSize(tsize,"x");
	atlasStyle->SetTitleSize(tsize,"x");
	atlasStyle->SetLabelSize(tsize,"y");
	atlasStyle->SetTitleSize(tsize,"y");
	atlasStyle->SetLabelSize(tsize,"z");
	atlasStyle->SetTitleSize(tsize,"z");


	//use bold lines and markers
	atlasStyle->SetMarkerStyle(20);
	atlasStyle->SetMarkerSize(1.2);
	atlasStyle->SetHistLineWidth(2);
	atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

	//get rid of X error bars and y error bar caps
	//atlasStyle->SetErrorX(0.001);

	//do not display any of the standard histogram decorations
	atlasStyle->SetOptTitle(0);
	//atlasStyle->SetOptStat(1111);
	atlasStyle->SetOptStat(0);
	//atlasStyle->SetOptFit(1111);
	atlasStyle->SetOptFit(0);

	// put tick marks on top and RHS of plots
	atlasStyle->SetPadTickX(1);
	atlasStyle->SetPadTickY(1);

	gROOT->SetStyle("ATLAS");

}

inline void DrawATLASLabel(float x,float y, float textsize = 0.05,bool Preliminary = true){
  TLatex l; //l.SetTextAlign(12);
  l.SetTextSize(textsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(1);

  double delx = 0.115*696*gPad->GetWh()/(472*gPad->GetWw());

  l.DrawLatex(x,y,"ATLAS");
  if (Preliminary) {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(1);
    p.DrawLatex(x+delx,y,"Preliminary");
    //    p.DrawLatex(x,y,"#sqrt{s}=900GeV");
  }
}



inline void DrawTitleLatex(const char* chartitle,
				float x, float y,
				int color = 1,
				float textsize = 0.04){
	std::string left = std::string(chartitle);
	std::vector<std::string> cuts;
	std::string::size_type found = 0;
	while(1){ // loop over the title, will exit with break
		left = left.substr(found);
		found = left.find(" - ");
		if(found == std::string::npos){
			cuts.push_back(left);
			break;
		}
		cuts.push_back(left.substr(0,found));
		found += 3; // take into account the delimiter " - "
	}
	TLatex TitleLabel;
	TitleLabel.SetNDC();
	TitleLabel.SetTextSize(textsize);
	TitleLabel.SetTextColor(color);
	for(unsigned int i = 0 ; i < cuts.size(); i++)
		TitleLabel.DrawLatex(x,y-i*(1.25)*textsize,(cuts[i]).c_str());

}

inline void DrawLegendLatex(const char* chartitle,
				int markertype,
				float x, float y,
				int color = 1,
				float textsize = 0.03){

	TMarker *markerdot = new TMarker(x - 0.4 * textsize, y + 0.2 * textsize, markertype);
	markerdot->SetNDC();
	markerdot->SetMarkerColor(color); 
  	markerdot->SetMarkerSize(25 * textsize);
	markerdot->Draw(); // cannot use DrawMarker because it does not SetNDC()
	//delete markerdot;
	
	DrawTitleLatex(chartitle, x, y, color, textsize);
}


inline void GetCoreParameters(const TH1 *hist, double &mean, double &mean_error,
					double &rms, double &rms_error, double &nentries){
	
	nentries = 0.;
	mean = 0.;
	mean_error = 0.;
	rms = 0.;
	rms_error = 0.;
	
	double axmin = hist->GetMean() - 3 * hist->GetRMS();
	double axmax = hist->GetMean() + 3 * hist->GetRMS();
	int nbins = hist->GetNbinsX();
	int imean = 0;
	hist->GetBinWithContent(hist->GetMean(), imean);
	float binwidth = hist->GetBinWidth(imean);
        if( fabs(axmax-axmin) < binwidth){
		nentries = hist->GetEntries();
	        mean = hist->GetMean();
	        rms = binwidth/sqrt(12);
        }else{
		for (int i = 0 ; i < nbins; i++){
			double value = hist->GetBinCenter(i+1);
			double ientries = hist->GetBinContent(i+1);
			if (value < axmin) continue;
			if (value > axmax) break;
			mean += value*ientries;
			rms += value*value*ientries;
			nentries += ientries;
		}
		if (nentries > 0) {
			mean = mean/nentries;
			rms = sqrt(rms/nentries-mean*mean);
		}
	}

	if (nentries > 0) {
		mean_error = rms / sqrt(nentries);
		if(nentries > 1) rms_error = rms / sqrt(2*(nentries-1));
	}
	
	return;
}
