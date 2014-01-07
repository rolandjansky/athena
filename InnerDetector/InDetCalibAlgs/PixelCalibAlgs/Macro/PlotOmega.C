/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::vector<std::string> filenames;
std::string parameter;
std::string axistitle;
double limit;
int layer;
std::vector<int> clustersizes;
std::vector<int> bins;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("PixelOfflineReco-Extended-G4");
//filenames.push_back("PixelOfflineReco-Extended-Repro4");
filenames.push_back("PixelOfflineReco-Extended-newOmega");
//filenames.push_back("PixelOfflineReco-Extended-ttbar");

//parameter = "Eta";
parameter = "Phi";

layer = 0;


clustersizes.push_back(2);
//clustersizes.push_back(3);


//bins.push_back(0);
//bins.push_back(25);
//bins.push_back(18);
bins.push_back(12);
//bins.push_back(-10);


///////////////////////////////////////////////////////////////////////////////////////
//
//  End of configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

	if(parameter == "Eta"){
	       	axistitle = "#Omega_{y}";
	}else{
		axistitle = "#Omega_{x}";
	}

TGaxis::SetMaxDigits(3);
gROOT->LoadMacro("../Macro/AtlasStyle.h");
std::string options;
std::string directory;
std::ostringstream histoname;

TCanvas *c1 = new TCanvas(parameter.c_str(),"",5);
c1->SetLeftMargin(0.14);

for(int ifile = 0;  ifile < filenames.size(); ifile++){
	TFile *file = TFile::Open((filenames[ifile] + ".root").c_str());
	file->cd();
	TDirectory *pippo = (TDirectory *)gDirectory->Get("ChargeInterpolation");
	pippo->cd();
	histoname.str(std::string());
	histoname << "m_Omega" << parameter << "h" << filenames[ifile];
	options = "";
	std::cout << histoname.str() <<  std::endl; 
	TH1F *swap = DrawHisto(histoname.str().c_str(),options.c_str(),1,1,0.15);
	swap->Scale(1/swap->GetEntries());
	swap->GetYaxis()->SetTitle("Fraction of pixel clusters");
	swap->GetYaxis()->SetTitleOffset(1.3);
	swap->GetXaxis()->SetTitle(axistitle.c_str());
	swap->Draw(options.c_str());
}
DrawATLASLabel(0.15,0.9);
}
