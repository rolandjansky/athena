/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::vector<std::string> filenames;
std::string parameter;
int clustersize;
bool G4;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("PixelOfflineReco-Extended-G4");
//filenames.push_back("PixelOfflineReco-Extended-G4largeETA");
//filenames.push_back("PixelOfflineReco-Extended-G4smallETA");
//filenames.push_back("PixelOfflineReco-Extended-G4smallALPHA");
//filenames.push_back("PixelOfflineReco-Extended-G4dig");
//filenames.push_back("PixelOfflineReco-Extended-G4largeETAdig");
//filenames.push_back("PixelOfflineReco-Extended-G4smallETAdig");
//filenames.push_back("PixelOfflineReco-Extended-G4smallALPHAdig");
//filenames.push_back("PixelOfflineReco-Extended-ttbar");
//filenames.push_back("PixelOfflineReco-Extended-new");
//filenames.push_back("../extended/PixelOfflineReco-Extended-newCUTS");
//filenames.push_back("../OffMC/PixelOfflineReco-Extended-MCcuts");
filenames.push_back("PixelOfflineReco-Extended-000-02");


//G4 = true;
G4 = false;

//parameter = "Eta";
parameter = "Phi";

clustersize = 0;
//clustersize = 1;
//clustersize = 2;
//clustersize = 3;
//clustersize = 4;
//clustersize = 5;
//clustersize = 6;

///////////////////////////////////////////////////////////////////////////////////////
//
//  End of configuration!
//
///////////////////////////////////////////////////////////////////////////////////////
TGaxis::SetMaxDigits(3);
gROOT->LoadMacro("../InnerDetector/InDetCalibAlgs/PixelCalibAlgs/PixelCalibAlgs-00-04-01/Macro/AtlasStyle.h");
std::ostringstream LegendString;
std::string options;
std::ostringstream directory;
std::string histoname;
int marker;
int color;
float legposition;
double mean;
double mean_error;
double rms;
double rms_error;
double nentries;
double maximum = -1000;
TH1F* first = 0;

std::string axistitle;
double limit;
std::ostringstream glbtitle;
std::ostringstream canvastitle;
glbtitle << clustersize;
int typecan = 1;
if(G4){
	typecan = 5;
	if(parameter == "Eta"){
		limit = 400;
	       	axistitle = "y_{truth} - y_{reco} [#mum]";
		glbtitle << "-col ";
		canvastitle << "yReso";
	}else{
		limit = 50;
		axistitle = "x_{truth} - x_{reco} [#mum]";
		glbtitle << "-row ";
		canvastitle << "xReso";
	}
}else{
	if(parameter == "Eta"){
		limit = 430;
	       	axistitle = "Local y residuals [#mum]";
		glbtitle << "-col ";
		canvastitle << "yResiduals";
	}else{
		limit = 100;
		axistitle = "Local x residuals [#mum]";
		glbtitle << "-row ";
		canvastitle << "xResiduals";
	}
}
glbtitle << "clusters";

if(clustersize > 0) canvastitle << clustersize;

TCanvas *c1 = new TCanvas(canvastitle.str().c_str(),"",typecan);
if(typecan ==5) c1->SetLeftMargin(0.14);
int iglobal = 0;

for(int idig = 0; idig < 2; idig++){
	for(int ifile = 0;  ifile < filenames.size(); ifile++){
		iglobal++;
		if(G4 && iglobal > 3) break;
		TFile *file = TFile::Open((filenames[ifile] + ".root").c_str());
		file->cd();
		TDirectory *pippo = (TDirectory *)gDirectory->Get("Validation");
		pippo->cd();
		directory.str(std::string());
		directory << "Res" << parameter;
		if(clustersize != 0) directory << "_Clustersize" << clustersize;
		if(idig == 0) directory << "Dig";
		std::cout << directory.str() << std::endl;
		pippo = (TDirectory *)gDirectory->Get(directory.str().c_str());
		pippo->cd();
		histoname = directory.str() + "_global";
		options = "P";
		if(iglobal > 1) options = "Psame";
		//if(G4) options+= "C";
		if(G4) marker = 20 + 3*ifile + 9*idig;
		else marker = 20 + 9*idig +4*ifile;
		if(marker > 30) marker = 30;
		if(G4) color = 2 - idig + 2*ifile;
		else color = 2 - idig;
		TH1F *histo = DrawHisto(histoname.c_str(),options.c_str(),color,marker,0.15);
		if(typecan ==5) histo->GetYaxis()->SetTitleOffset(1.4);
		GetCoreParameters(histo, mean, mean_error, rms, rms_error, nentries);
		histo->Sumw2();	
		histo->Scale(1/histo->GetEntries());
		histo->GetYaxis()->SetTitle("Fraction of pixel clusters");
		//histo->GetXaxis()->SetRangeUser(-limit,limit);
		histo->GetXaxis()->SetTitle(axistitle.c_str());
		float newmax = histo->GetMaximum();
		if(maximum < newmax) maximum = newmax;
		LegendString.str(std::string());
		LegendString.flags(std::ios::fixed);
		if(idig == 0) LegendString << "Center of the cluster - ";
		else LegendString << "Charge sharing algorithm - ";
		int precision = 1-int(log10(rms_error));
		if(precision < 0) precision = 0;
		else if(precision > 3){
			LegendString.flags(std::ios::scientific);
			precision = 2;
		}
		LegendString.precision(precision);
		LegendString << "RMS = " << rms << " #pm " << rms_error << " #mum" << " - ";
		precision = 1-int(log10(mean_error));
		if(precision < 0) precision = 0;
		else if(precision > 3){
			LegendString.flags(std::ios::scientific);
			precision = 2;
		}
		//LegendString.precision(precision);
		//LegendString << "Mean = " <<  mean << " #pm " << mean_error << "#mum" << " - ";
		legposition = 0.9 - 0.1 * (iglobal-1);
		float legxpos = 0.65;
		if(G4) legxpos = 0.61;
		DrawLegendLatex(LegendString.str().c_str(), marker, legxpos, legposition, color,0.035);
		if(iglobal==1) first = histo;
	}	
}
if(first){
	if(parameter == "Eta" && !G4) maximum = 10.*maximum/7;
	else maximum = 10.*maximum/8;
	first->SetMaximum(maximum);
}
if(clustersize != 0) DrawTitleLatex(glbtitle.str().c_str(), 0.2, 0.75);
DrawATLASLabel(0.15,0.9);

}
