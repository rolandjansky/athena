/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::vector<std::string> filenames;
std::string parameter;
std::string MorRMS;
bool G4;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("PixelOfflineReco-Extended-COS-G4");
//filenames.push_back("PixelOfflineReco-Extended-COS-G4-ttbar");
//filenames.push_back("PixelOfflineReco-Extended-G4dig");
//filenames.push_back("../ttbar/PixelOfflineReco-Extended-ttbar");
//filenames.push_back("PixelOfflineReco-Extended-MC");
//filenames.push_back("PixelOfflineReco-Extended-Repro4");
//filenames.push_back("PixelOfflineReco-100GeVmuons");
//filenames.push_back("PixelOfflineReco-100GeVmuons_comm");
//filenames.push_back("PixelOfflineReco-900GeV_comm");
//filenames.push_back("PixelOfflineReco-MC_900Gev_day1");
//filenames.push_back("PixelOfflineReco-MC_900Gev_day1_comm");
//filenames.push_back("PixelOfflineReco-MC_900Gev_nom");
//filenames.push_back("PixelOfflineReco-MC_900Gev_nom_comm");
filenames.push_back("PixelOfflineReco-Extended-000-02");



//G4 = true;
G4 = false;

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
gROOT->LoadMacro("../InnerDetector/InDetCalibAlgs/PixelCalibAlgs/PixelCalibAlgs-00-04-01/Macro/AtlasStyle.h");
std::string directory;
std::string options;
std::ostringstream LegendString;
int marker;
int color;
float legposition;
double maximum = -1000;
double minimum = 1000;
TH1F* first = 0;

TCanvas *c1 = new TCanvas((parameter+"profile").c_str(),"",5);
c1->SetLeftMargin(0.14);
c1->SetGridy();
int iglobal = 0;

std::string Yaxistitle;
std::string Xaxistitle;
if(G4){
	if(parameter == "Eta"){
	       	Yaxistitle = "Local y resolution [#mum]";
	       	Xaxistitle = "#eta_{i}";
	}else{
		Yaxistitle = "Local x resolution  [#mum]";
	       	Xaxistitle = "#phi_{i} [#circ]";
	}
}else{
	if(parameter == "Eta"){
	       	Yaxistitle = "RMS of local y residuals [#mum]";
	       	Xaxistitle = "#eta_{i}";
	}else{
		Yaxistitle = "RMS of local x residuals [#mum]";
	       	Xaxistitle = "#phi_{i} [#circ]";
	}
}


for(int idig = 0; idig < 2; idig++){
	for(int ifile = 0;  ifile < filenames.size(); ifile++){
		iglobal++;
		TFile *file = TFile::Open((filenames[ifile] + ".root").c_str());
		file->cd();
		TDirectory *pippo = (TDirectory *)gDirectory->Get("Validation");
		pippo->cd();
		directory = "Res" + parameter;
		if(idig == 0) directory += "Dig";
		pippo = (TDirectory *)gDirectory->Get(directory.c_str());
		pippo->cd();
		std::string histo = directory + "_" + MorRMS +  "_vs_";
		if(parameter == "Eta"){
			histo += "eta";
		}else{
			histo += "phi";
		}
		std::cout << histo << std::endl;
		options = "P";
		if(iglobal > 1) options = "Psame";
		if(G4) marker = 20 + 3*ifile + 9*idig;
		else marker = 20 + 9*idig +4*ifile;
		if(marker > 30) marker = 30;
		if(G4) color = 2 - idig + 2*ifile;
		else color = 2 - idig;
		//color = ifile +1;
		if(color ==3) color =4;
		marker =  20 + iglobal;
		if(idig > 0) marker = 29;
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
		//if(idig == 0) LegendString << "Center of the cluster - ";
		//else LegendString << "Charge sharing algorithm - ";
		LegendString << filenames[ifile];
		DrawLegendLatex(LegendString.str().c_str(), marker, 0.55, legposition, color);
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

DrawATLASLabel(0.15,0.9);

}
