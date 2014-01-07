/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
std::vector<std::string> filenames;
std::string parameter;
std::string MorRMS;
bool G4;
bool TH;
int layern;
float p1,p2;

///////////////////////////////////////////////////////////////////////////////////////
//
//  Here is configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

//filenames.push_back("G4ana4nov/PixelOfflineReco-Extended-G4");
//filenames.push_back("G4ana4nov/PixelOfflineReco-Extended-G4dig");
//filenames.push_back("ttbar/PixelOfflineReco-Extended-ttbar");
//filenames.push_back("PixelOfflineReco-Extended-Repro4nocuts");
//filenames.push_back("PixelOfflineReco-Extended-Repro4");
//filenames.push_back("PixelOfflineReco-Extended-Repro4cut5GeV");
filenames.push_back("PixelOfflineReco-Extended-000-02");


//G4 = true;
G4 = false;

//TH = false;
TH = true;

parameter = "Eta"; p2 = 1000; p1 = 400;
//parameter = "Phi";  p2 = 1000; p1 = 80;

layern = 0;

//MorRMS = "Mean";
MorRMS = "RMS";


///////////////////////////////////////////////////////////////////////////////////////
//
//  End of configuration!
//
///////////////////////////////////////////////////////////////////////////////////////

TGaxis::SetMaxDigits(3);
gROOT->LoadMacro("../../InnerDetector/InDetCalibAlgs/PixelCalibAlgs/PixelCalibAlgs-00-04-01/Macro/AtlasStyle.h");
std::ostringstream directory;
std::string options;
std::ostringstream LegendString;
int marker;
int color;
float legposition;
double maximum = -1000;
double minimum = 1000;
TH1F* first = 0;
std::ostringstream name;
name << parameter << "PTcut";
if(TH) name << 2;

TCanvas *c1 = new TCanvas(name.str().c_str(),"",5);

c1->SetLeftMargin(0.14);
if(TH) c1->SetRightMargin(0.15);
c1->SetGridy();
c1->SetLogx();
int iglobal = 0;
gStyle->SetPalette(1);

std::string Yaxistitle;
std::string Xaxistitle = "p_{T} [GeV]";
if(G4){
	if(parameter == "Eta"){
	       	if(!TH) Yaxistitle = "Local y resolution [#mum]";
	        else Yaxistitle = "y_{truth} - y_{reco} [#mum]";
	}else{
		if(!TH) Yaxistitle = "Local x resolution  [#mum]";
	        else Yaxistitle = "x_{truth} - x_{reco} [#mum]";
	}
}else{
	if(parameter == "Eta"){
	       	if(!TH) Yaxistitle = "RMS of local y residuals [#mum]";
	        else Yaxistitle = "Local y residuals [#mum]";
	}else{
		if(!TH) Yaxistitle = "RMS of local x residuals  [#mum]";
		else Yaxistitle = "Local x residuals [#mum]";
	}
}


for(int idig = 0; idig < 1; idig++){
	for(int ifile = 0;  ifile < filenames.size(); ifile++){
		iglobal++;
		TFile *file = TFile::Open((filenames[ifile] + ".root").c_str());
		file->cd();
		TDirectory *pippo = (TDirectory *)gDirectory->Get("Validation");
		pippo->cd();
		directory.str(std::string());				
		directory << "Res" << parameter;
	        if(!TH) directory << "_Layer" << layern;
		if(idig == 0) directory << "Dig";
		if(!TH){
			pippo = (TDirectory *)gDirectory->Get(directory.str().c_str());
			pippo->cd();
		}
		std::string histo = directory.str();
		if(!TH) histo += "_" + MorRMS;
	       	histo += "_vs_p";
		std::cout << histo << std::endl;
		options = "P";
		if(iglobal > 1) options = "Psame";
		marker = 20 + 3*ifile + 9*idig; 
		color = 2 - idig + 2*ifile;
		if(TH) options = "COLZ";
		TH1F *histogram = DrawHisto(histo.c_str(),options.c_str(),color,marker,0.15);
		histogram->GetXaxis()->SetTitleOffset(1.3);	
		histogram->GetXaxis()->SetTitle(Xaxistitle.c_str());	
		histogram->GetYaxis()->SetTitleOffset(1.3);	
		histogram->GetYaxis()->SetTitle(Yaxistitle.c_str());	
		if(TH){
		       histogram->Scale(1/histogram->GetEntries());
		       histogram->Draw("COLZ");
		}
		if(!TH){
			float newmin = histogram->GetMinimum();
			float newmax = histogram->GetMaximum();
			if(maximum < newmax) maximum = newmax;
			if(newmin < minimum) minimum = newmin;
			legposition = 0.9 - 0.05 * iglobal;
			LegendString.str(std::string());		
			//if(ifile == 0) LegendString << "All tracks";
			//else
			LegendString << filenames[ifile];
			DrawLegendLatex(LegendString.str().c_str(), marker, 0.55, legposition, color);
			if(iglobal==1) first = histogram;
		}
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
	if(!TH) first->GetXaxis()->SetRangeUser(0.5,2e3);	
}

if(TH){
	TF1 *cut = new TF1("cut","sqrt([0]*[0] + [1]*[1]/(x*x))",4,1e4);
	TF1 *cut2 = new TF1("cut2","-sqrt([0]*[0] + [1]*[1]/(x*x))",4,1e4);
	cut->SetParameter(0,p1);
	cut->SetParameter(1,p2);
	cut->DrawCopy("same");
	cut2->SetParameter(0,p1);
	cut2->SetParameter(1,p2);
	cut2->DrawCopy("same");
	TLine *line1 =  new TLine();
	line1->SetLineWidth(3);
	//line1->SetLineStyle(3);
	line1->DrawLine(5,cut->Eval(4),5,cut2->Eval(4));
}


DrawATLASLabel(0.15,0.9);

}
