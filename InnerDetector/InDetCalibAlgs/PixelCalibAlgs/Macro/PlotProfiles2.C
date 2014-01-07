/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::vector<std::string> filenames;
std::string parameter;
std::string MorRMS;
bool G4;
bool dig;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("PixelOfflineReco-Extended-G4");
//filenames.push_back("PixelOfflineReco-Extended-G4dig");
//filenames.push_back("../ttbar/PixelOfflineReco-Extended-ttbar");
//filenames.push_back("PixelOfflineReco-Extended-Repro4cut5GeV");
//filenames.push_back("PixelOfflineReco-Extended-new");
//filenames.push_back("../OffMC/PixelOfflineReco-Extended-MC");
//filenames.push_back("PixelOfflineReco-MC_900Gev_nomERR");
//filenames.push_back("PixelOfflineReco-MC_900Gev_nom_comm");
//filenames.push_back("PixelOfflineReco-900GeV_comm");
filenames.push_back("PixelOfflineReco-Extended-COS-G4-ttbar");
filenames.push_back("PixelOfflineReco-Extended-COS-G4");


G4 = true;
//G4 = false;

dig = true;
//dig = false;

//parameter = "Eta";
parameter = "Phi";

//MorRMS = "Mean";
MorRMS = "RMS";


///////////////////////////////////////////////////////////////////////////////////////
//
//  End of configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

TGaxis::SetMaxDigits(3);
gROOT->LoadMacro("../Macro/AtlasStyle.h");
std::ostringstream directory;
std::string legendtitle;
std::string options;
std::ostringstream LegendString;
int marker;
int color;
float legposition;
double maximum = -1000;
double minimum = 1000;
TH1F* first = 0;

TCanvas *c1 = new TCanvas(parameter.c_str(),"",5);
c1->SetLeftMargin(0.14);
c1->SetGridy();
int iglobal = 0;

std::string Yaxistitle;
std::string Xaxistitle;
if(G4){
	if(parameter == "Eta"){
	       	Yaxistitle = "Local y resolution [#mum]";
	       	Xaxistitle = "#eta_{i}";
		legendtitle = "-col clusters";
	}else{
		Yaxistitle = "Local x resolution  [#mum]";
	       	Xaxistitle = "#phi_{i} [#circ]";
		legendtitle = "-row clusters";
	}
}else{
	if(parameter == "Eta"){
	       	Yaxistitle = "RMS of local y residuals [#mum]";
	       	Xaxistitle = "#eta_{i}";
		legendtitle = "-col clusters";
	}else{
		Yaxistitle = "RMS of local x residuals [#mum]";
	       	Xaxistitle = "#phi_{i} [#circ]";
		legendtitle = "-row clusters";
	}
}


for(int iclustersize = 0; iclustersize < 5; iclustersize++){
	for(int ifile = 0;  ifile < filenames.size(); ifile++){
		iglobal++;
		TFile *file = TFile::Open((filenames[ifile] + ".root").c_str());
		file->cd();
		TDirectory *pippo = (TDirectory *)gDirectory->Get("Validation");
		pippo->cd();
		directory.str(std::string());
		directory << "Res" << parameter << "_Clustersize" << iclustersize+1;
		if(dig) directory << "Dig";
		std::cout << directory.str() << std::endl;
		pippo = (TDirectory *)gDirectory->Get(directory.str().c_str());
		pippo->cd();
		std::string histo = directory.str() + "_" + MorRMS +  "_vs_";
		if(parameter == "Eta"){
			histo += "eta";
		}else{
			histo += "phi";
		}
		std::cout << histo << std::endl;
		options = "P";
		if(iglobal > 1) options = "Psame";
		marker = 20 + iclustersize + 4 * ifile; 
		color = 0 + 2*iclustersize;
		if(color == 0) color = 1;
		TH1F *histogram = DrawHisto(histo.c_str(),options.c_str(),color,marker,0.15);
		float newmin = histogram->GetMinimum();
		float newmax = histogram->GetMaximum();
		if(maximum < newmax) maximum = newmax;
		if(newmin < minimum) minimum = newmin;
		histogram->GetYaxis()->SetTitleOffset(1.3);	
		histogram->GetYaxis()->SetTitle(Yaxistitle.c_str());	
		histogram->GetXaxis()->SetTitle(Xaxistitle.c_str());	
		legposition = 0.95 - 0.05 * iglobal;
		LegendString.str(std::string());		
		LegendString <<iclustersize+1 << legendtitle;
		DrawLegendLatex(LegendString.str().c_str(), marker, 0.57, legposition, color);
		if(iglobal==1) first = histogram;
	}
}

if(first){
	if(MorRMS == "RMS"){
	       	minimum = 0;
		maximum = 10.*maximum/6.5;
	}else{
		minimum = 10.*minimum/3.5;
		maximum = 10.*maximum/2.5;
	}
	first->SetMinimum(minimum);
	first->SetMaximum(maximum);
}

if(dig) DrawTitleLatex("Center of the cluster", 0.2, 0.75);
else DrawTitleLatex("Charge sharing algorithm", 0.2, 0.75);
DrawATLASLabel(0.15,0.9);

}
