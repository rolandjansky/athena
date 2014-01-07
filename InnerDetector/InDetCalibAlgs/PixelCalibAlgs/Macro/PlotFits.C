/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::vector<std::string> filenames;
std::string parameter;
bool G4;
int layer;
std::vector<int> clustersizes;
std::vector<double> bins;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("PixelOfflineReco-Extended-G4");
//filenames.push_back("PixelOfflineReco-Extended-G4dig");
//filenames.push_back("../ttbar/PixelOfflineReco-Extended-ttbar");
filenames.push_back("PixelOfflineReco-Extended-000-02");

//G4 = true;
G4 = false;

parameter = "eta";
//parameter = "phi";

layer = 0;


clustersizes.push_back(2);
//clustersizes.push_back(3);


//bins.push_back(0);
//bins.push_back(25);
//bins.push_back(18);
bins.push_back(-1.5);
//bins.push_back(-10);

///////////////////////////////////////////////////////////////////////////////////////
//
//  End of configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

TGaxis::SetMaxDigits(3);
gROOT->LoadMacro("~/ATHENA/15.6.8.5/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/PixelCalibAlgs-00-04-01/Macro/AtlasStyle.h");
std::string directory;
std::string options;
std::ostringstream LegendString;
int marker;
int color;
float legposition;
double maximum = -1000;
double minimum = 1000;
TH1F* first = 0;

std::ostringstream name;
name << "FitExample" << parameter << bins[0];

TCanvas *c1 = new TCanvas(name.str().c_str(),"",5);
c1->SetLeftMargin(0.14);
c1->SetGridy();
int iglobal = 0;

std::string Yaxistitle;
std::string Xaxistitle;
if(G4){
	if(parameter == "eta"){
	       	Yaxistitle = "y_{truth} - y_{center} [#mum]";
	       	Xaxistitle = "#Omega_{y}";
	}else{
		Yaxistitle = "x_{truth} - x_{center} [#mum]";
	       	Xaxistitle = "#Omega_{x}";
	}
}else{
	if(parameter == "eta"){
	       	Yaxistitle = "Local y residuals [#mum]";
	       	Xaxistitle = "#Omega_{y}";
	}else{
		Yaxistitle = "Local x residuals [#mum]";
	       	Xaxistitle = "#Omega_{x}";
	}
}


for(int ics = 0; ics < clustersizes.size(); ics++){
	for(int ibin = 0;  ibin < bins.size(); ibin++){
		iglobal++;
		TFile *file = TFile::Open((filenames[0] + ".root").c_str());
		file->cd();
		TDirectory *pippo = (TDirectory *)gDirectory->Get("ChargeInterpolation");
		pippo->cd();
		directory = parameter + "ResVsOmega" + filenames[0];
		std::cout << directory << std::endl;
		pippo = (TDirectory *)gDirectory->Get(directory.c_str());
		pippo->cd();
		std::ostringstream histo;
	        histo << directory << "_Layer" << layer << "-" << layer+1
			<< "_" << parameter << bins[ibin] << "-";
		if(parameter == "eta") histo << bins[ibin] + 0.5;
		else if(bins[ibin] < 20) histo << bins[ibin] + 2;
		else histo << bins[ibin] + 5;
		histo << "_ClusterSize" << clustersizes[ics] - 0.5 << "-"
		       	<< clustersizes[ics] + 0.5;
		std::cout << histo.str() << std::endl;
		options = "P";
		if(iglobal > 1) options = "Psame";
		marker = 20 + 3*ibin + 9*ics;
		color = 1;
		if(G4) color = 2 - ics + 2*ibin;

		TH1F *histogram = DrawHisto(histo.str().c_str(),options.c_str(),color,marker,0.15);
		float newmin = histogram->GetMinimum();
		float newmax = histogram->GetMaximum();
		if(maximum < newmax) maximum = newmax;
		if(newmin < minimum) minimum = newmin;
		histogram->GetYaxis()->SetTitleOffset(1.3);	
		histogram->GetYaxis()->SetTitle(Yaxistitle.c_str());	
		histogram->GetXaxis()->SetTitle(Xaxistitle.c_str());	
		legposition = 0.9 - 0.05 * iglobal;
		LegendString.str(std::string());		
		LegendString << clustersizes[ics];
		if(parameter == "eta") LegendString << "-col clusters  ";
		else LegendString << "-row clusters  ";
		if(!G4) LegendString << " - ";
		LegendString << bins[ibin] << " < #" << parameter << "_{i} <";
		if(parameter == "eta") LegendString << bins[ibin] + 0.5;
		else if(bins[ibin] < 20) LegendString << bins[ibin] + 2;
		else LegendString << bins[ibin] + 5;
		float size = 0.04;
		float xpos = 0.65;
		if(G4){
		       	size = 0.034;
			xpos = 0.57;
		}
		DrawLegendLatex(LegendString.str().c_str(), marker, xpos, legposition, color,
				size);
		if(iglobal==1) first = histogram;
	}
}

if(first){
	//minimum = 10.*minimum/7.5;
	//maximum = 10.*maximum/5.5;
	minimum = -40;
	maximum = 60;
	first->SetMinimum(minimum);
	first->SetMaximum(maximum);
}

DrawATLASLabel(0.15,0.9);

}
