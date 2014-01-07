/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>
#include <string.h>

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
	atlasStyle->SetFrameFillColor(icol);
	atlasStyle->SetCanvasBorderMode(icol);
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

inline void DrawATLASLabel(float x,float y, bool pre = false , float textsize = 0.05){
	TLatex ATLASLabel;
	ATLASLabel.SetNDC();
	ATLASLabel.SetTextFont(72);
	ATLASLabel.SetTextSize(textsize);
	//if(pre)
	       	//ATLASLabel.DrawLatex(x,y,"ATLAS");
	       	//ATLASLabel.DrawLatex(x,y-0.05,"work in progress");
	//else ATLASLabel.DrawLatex(x,y,"ATLAS");
}


inline void DrawTitleLatex(const char* chartitle,
				float x, float y,
				int color = 1,
				float textsize = 0.04){
        char null = '\0';
	TLatex TitleLabel;
	TitleLabel.SetNDC();
	TitleLabel.SetTextSize(textsize);
	TitleLabel.SetTextColor(color);

	int i = 2; // understand this if you can...
	int offset = 0;
	int counter = 0;
	char swap[100];
	for(int i = 2; ; i++  ){ // will exit at the end of the string...	
		if(chartitle[i-2] == ' '  && chartitle[i-1] == '-' && chartitle[i] == ' '){
			strncpy(swap, chartitle + offset, (i-2 - offset)*sizeof(char));
			swap[(i-2 - offset)] = null ;
			TitleLabel.DrawLatex(x, y-counter*(1.25)*textsize, swap);
			offset = i+1;
			counter++;
		}else if(chartitle[i] == null ){
			TitleLabel.DrawLatex(x, y-counter*(1.25)*textsize, chartitle + offset);
			break;
		
		}
	}
}


inline void DrawLegendLatex(const char* chartitle,
				int markertype,
				float x, float y,
				int color = 1,
				float textsize = 0.034){

	TMarker *markerdot = new TMarker(x - 0.5 * textsize, y + 0.3 * textsize, markertype);
	markerdot->SetNDC();
	markerdot->SetMarkerColor(color); 
  	markerdot->SetMarkerSize(40 * textsize);
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

	for (int i = 0 ; i < nbins; i++){
		double value = hist->GetBinCenter(i+1);
		double ientries = hist->GetBinContent(i+1);
		if (value <= axmin) continue;
		if (value >= axmax) break;
		mean += value*ientries;
		rms += value*value*ientries;
		nentries += ientries;
	}

	if (nentries > 0) {
		mean = mean/nentries;
		rms = sqrt(rms/nentries-mean*mean);
		mean_error = rms / sqrt(nentries);
		if(nentries > 1) rms_error = rms / sqrt(2*(nentries-1));
	}
	
	return;
}


TH1 *DrawHisto(const char *name, const char* options = "", int color = 1, int marker = 20, float offset = 0){
	TH1F *histo = (TH1F *)gDirectory->Get(name);
	if(!histo) return 0;
	histo->SetDirectory(gROOT);
	histo->UseCurrentStyle();
	histo->SetLineColor(color);
	//histo->SetFillColor(color);
	histo->SetMarkerColor(color);
	histo->SetMarkerSize(1.8);
	histo->SetMarkerStyle(marker);
	histo->Draw(options);
	if(offset == 0) DrawTitleLatex(histo->GetTitle(), 0.2, 0.85);
	//DrawLegendLatex(histo->GetTitle(), marker, 0.7, 0.85 - offset ,color);
	return histo;

}

TH1 *DrawOneResidualDist(std::string name, int color, int marker, float offset){
	double mean;
	double mean_error;
	double rms;
	double rms_error;
	double nentries;

	std::string options = "P";
	if(offset != 0) options = "Psame";
	TH1 *histo = DrawHisto(name.c_str(),options.c_str(),color,marker,offset);
	GetCoreParameters(histo, mean, mean_error, rms, rms_error, nentries);
	histo->Scale(1/histo->GetEntries());
	histo->GetYaxis()->SetTitle("Fraction of pixel clusters");
	histo->GetXaxis()->SetTitle("Residual [#mum]");	
	std::ostringstream LegendString1;
	LegendString1.flags(std::ios::fixed);
	LegendString1 << name << " - ";
	int precision = 1-int(log10(rms_error));
	if(precision < 0) precision = 0;
	LegendString1.precision(precision);
	LegendString1 << "RMS: " << rms << " #pm " << rms_error << " #mum" << " - ";
	precision = 1-int(log10(mean_error));
	if(precision < 0) precision = 0;
	LegendString1.precision(precision);
	LegendString1 << "Mean: " <<  mean << " #pm " << mean_error << " #mum" << " - ";
	DrawLegendLatex(LegendString1.str().c_str(), marker, 0.7, 0.85 - offset ,color);
}

TH1 *DrawOneResidualProfile(std::string name, int color, int marker, float offset){
	std::string options = "P";
	if(offset != 0) options = "Psame";
	TH1 *histo = DrawHisto(name.c_str(),options.c_str(),color,marker,offset);
	histo->GetYaxis()->SetTitle("RMS of residuals [#mum]");
	//histo->GetXaxis()->SetTitle("Residual [#mum]");	
	std::ostringstream LegendString1;
	DrawLegendLatex(name.c_str(), marker, 0.6, 0.9 - offset ,color);
}
