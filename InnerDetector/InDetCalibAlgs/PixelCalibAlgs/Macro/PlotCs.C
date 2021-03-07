/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifdef __CPPCHECK__
void PlotCs()
#endif
{
std::vector<std::string> filenames;
std::string parameter;
std::string parameter2;
std::string axistitle;
std::string legendtitle;
double limit;
double xrange;
double yrange;
std::string type;
bool sum;
int maxcs;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("PixelOfflineReco-Extended-G4");
//filenames.push_back("PixelOfflineReco-Extended-Repro4nocuts");
//filenames.push_back("PixelOfflineReco-Extended-new");
//filenames.push_back("PixelOfflineReco-MC_900Gev_nom");
//filenames.push_back("PixelOfflineReco-900GeV_comm");
//filenames.push_back("PixelOfflineReco-Extended-ttbar");
filenames.push_back("PixelOfflineReco-Extended-000-02");

sum = true;
//sum = false;

parameter = "Eta";
//parameter = "Phi";

//type = "G4";
//type = "Coll";


maxcs = 5;
//maxcs = 10;

///////////////////////////////////////////////////////////////////////////////////////
//
//  End of configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

	if(parameter == "Eta"){
	       	axistitle = "#eta_{i}";
		legendtitle = "-col clusters";
		xrange = 2.5;
		yrange = 0.2;
	}else{
		axistitle = "#phi_{i} [#circ]";
		legendtitle = "-row clusters";
		xrange = 25;
		yrange = 0.25;
	}
		//if(type == "Coll") yrange = 0.12; 

TGaxis::SetMaxDigits(3);
gROOT->LoadMacro("../../InnerDetector/InDetCalibAlgs/PixelCalibAlgs/PixelCalibAlgs-00-04-01/Macro/AtlasStyle.h");
std::ostringstream LegendString;
std::string options;
std::string directory;
std::ostringstream histoname;
int marker;
float legposition;
TH1F *histo[10];

TCanvas *c1 = new TCanvas(parameter.c_str(),"",5);
c1->SetLeftMargin(0.14);
int iglobal = 0;

size_t nfilenames = filenames.size();
for(int ifile = 0;  ifile < nfilenames; ++ifile){
	for(int iCS = 0; iCS < maxcs; iCS++){
		iglobal++;
		TFile *file = TFile::Open((filenames[ifile] + ".root").c_str());
		file->cd();
		TDirectory *pippo = (TDirectory *)gDirectory->Get("Validation");
		pippo->cd();
		histoname.str(std::string());
		histoname << parameter << "_Clustersize" << iCS+1;
		options = "";
		std::cout << histoname.str() <<  std::endl; 
		TH1F *swap = DrawHisto(histoname.str().c_str(),options.c_str(),1,marker,0.15);
		swap->GetYaxis()->SetTitle("Fraction of pixel clusters");
		swap->GetYaxis()->SetTitleOffset(1.3);
		swap->GetXaxis()->SetTitle(axistitle.c_str());
		if(iCS != 0 && sum) swap->Add(histo[iCS-1]);
		histo[iCS] = swap;
	}	
}
options = "";
TH1F *first = histo[0];
for(int iCS = 0; iCS < maxcs; iCS++){
	int index = iCS;
	if(sum) index = maxcs-1-iCS;
	int color = index+2;
	if(color == 10) color = 1;
        // cppcheck-suppress arrayIndexOutOfBoundsCond; false positive
	TH1F *swap = histo[index];
	if(sum) swap->Scale(1/first->GetEntries());
	swap->Draw(options.c_str());
	swap->SetFillColor(color);
	swap->SetLineColor(color);
	options = "same";
	LegendString.str(std::string());
	LegendString <<index+1 << legendtitle;
	float xpos = 0.53;
	legposition = 0.9 - 0.05 * (index);
	if(index > 4){
	       xpos = 0.75;
	       legposition += 5*0.05;
	}
	DrawTitleLatex(LegendString.str().c_str(), xpos, legposition, color, 0.035);
	swap->GetYaxis()->SetRangeUser(0,yrange);
	swap->GetXaxis()->SetRangeUser(-xrange,xrange);
}
DrawATLASLabel(0.15,0.9);

TLatex Label;
Label.SetNDC();
Label.SetTextFont(72);
Label.SetTextSize(0.05);
if(type == "G4"){
	Label.DrawLatex(0.2,0.75,"Simulated");
	Label.DrawLatex(0.2,0.7,"cosmic rays");
}else if(type == "Coll"){
	Label.DrawLatex(0.2,0.75,"Simulated");
	Label.DrawLatex(0.2,0.7,"collisions");
}

}
