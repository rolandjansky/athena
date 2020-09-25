/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationPlot_cxx
#define PixelChargeInterpolationPlot_cxx

#include <vector>
#include <string>
#include <sstream>
#include <atomic>

#include <TH1.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TGaxis.h>

#include "PixelCalibAlgs/PixelChargeInterpolationPlot.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include "PixelCalibAlgs/AtlasStyle.h"

namespace PixelCalib{

// number of layers in barrel & disks
const int PixelChargeInterpolationPlot::m_nlayers = 3;

PixelChargeInterpolationPlot::PixelChargeInterpolationPlot(
		const PixelChargeInterpolationParameters &parameters, bool oneconst):
	m_histogramsX(0), m_histogramsY(0),
	m_RefHistosX(0), m_RefHistosY(0),
	m_etabins(0), m_phibins(0), m_netabins(0), m_nphibins(0),
	m_referenceDrawOpt(0), m_oneconst(oneconst){

//	std::cout << "===============================================" << std::endl;
//	std::cout << "What we get to plot..." << std::endl;
//	std::cout << "x constants: " << parameters.getNumberOfXbins() << std::endl;
//	std::cout << "x constants: " << parameters.getAngleBins().size() << " "
//		<< parameters.getClusterSizeXBins().size() << std::endl;
//	for(unsigned int i = 0; i <  parameters.getAngleBins().size(); i++){
//		std::cout << (parameters.getAngleBins())[i] << std::endl;
//	}
//	std::cout << "y constants: " << parameters.getNumberOfYbins() << std::endl;
//	std::cout << "y constants: " << parameters.getEtaBins().size() << " "
//		<< parameters.getClusterSizeYBins().size() << std::endl;
//	std::cout << "===============================================" << std::endl;

		
	SetAtlasStyle();
	m_referenceDrawOpt =  new std::vector<std::string>();
	m_RefHistosX = new std::vector < std::vector < TH1F* > >();
	m_RefHistosY = new std::vector < std::vector < TH1F* > >();
	m_histogramsX = HistogramsFromConstants(parameters,"phi",2);
	m_histogramsY = HistogramsFromConstants(parameters,"eta",2);

}

//////////////////////////////////////////////////////////////////////////////////////////

PixelChargeInterpolationPlot::~PixelChargeInterpolationPlot(){

	delete m_histogramsX;
	delete m_histogramsY;
	delete m_referenceDrawOpt;
	delete m_RefHistosX;
	delete m_RefHistosY;
	delete[] m_etabins;
	delete[] m_phibins;

	m_histogramsX = 0;
	m_histogramsY = 0;
	m_RefHistosX = 0;
	m_RefHistosY = 0;
	m_etabins = 0;
	m_phibins = 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationPlot::AddReference(
		const PixelChargeInterpolationParameters &parameters,
		const std::string& title, int color, const std::string& drawopt){

	std::vector < TH1F* > *histoX = HistogramsFromConstants(parameters,"phi",color,title);
	std::vector < TH1F* > *histoY = HistogramsFromConstants(parameters,"eta",color,title);

	m_referenceDrawOpt->push_back( drawopt + std::string("same") );
	m_RefHistosX->push_back(*histoX);
	m_RefHistosY->push_back(*histoY);
	
	delete histoX;
	delete histoY;

}

//////////////////////////////////////////////////////////////////////////////////////////
	
void PixelChargeInterpolationPlot::Plot(const std::string& output){


	PlotDirection(output, "phi");
	PlotDirection(output, "eta");
	
}
	
//////////////////////////////////////////////////////////////////////////////////////////

std::vector < TH1F* > *PixelChargeInterpolationPlot::HistogramsFromConstants(
			const PixelChargeInterpolationParameters &parameters,
			const std::string& direction, int color, const std::string& title){

	std::vector <TH1F*> *histo = new std::vector <TH1F*>();
	std::vector<float> anglebins;
	std::vector<float> csbins;
	int nConstants = 0;
	std::string xory, colorrow;

	if(direction == "phi"){
		anglebins = parameters.getAngleBins();
		csbins = parameters.getClusterSizeXBins();
		nConstants = parameters.getNumberOfXbins();
		if(m_phibins == 0 && m_nphibins == 0){
			m_nphibins = anglebins.size();
			m_phibins = new double[m_nphibins];
			for(int i = 0; i < m_nphibins; i++)
				m_phibins[i] = anglebins[i];
		}
		xory = "x";
		colorrow = "row"; 

	}else{
		anglebins = parameters.getEtaBins();
		csbins = parameters.getClusterSizeYBins();
		nConstants = parameters.getNumberOfYbins();
		if(m_etabins == 0 && m_netabins == 0){
			m_netabins = anglebins.size();
			m_etabins = new double[m_netabins];
			for(int i = 0; i < m_netabins; i++)
				m_etabins[i] = anglebins[i];
		}
		xory = "y";
		colorrow = "col";
	}
	
	int nangleBins = anglebins.size() - 1;
	int nCsBins = csbins.size() - 1;
	int nLayerBins = nConstants / (nangleBins * nCsBins);

	int nhisto = nCsBins;
	int nbinhisto = nLayerBins * nangleBins;

//	std::cout << "===============================================" << std::endl;
//	std::cout <<  "PixelChargeInterpolationPlot" << std::endl;
//	std::cout << "The histogram... " << direction << std::endl;
//	std::cout << "Parameters version: " <<  parameters.getVersion() << std::endl;
//	std::cout << "n constants: " << nConstants << std::endl;
//	std::cout << "n angle bins: " << nangleBins << std::endl;
//	std::cout << "n cs bins: " << nCsBins << std::endl;
//	std::cout << "n layer bins: " << nLayerBins << std::endl;
//	std::cout << "n histos: " << nhisto << std::endl;
//	std::cout << "n bins: " << nbinhisto << std::endl;
//	std::cout << "===============================================" << std::endl;

	static std::atomic<int> pass = 0;
        int p = pass++;
	for(int i = 0; i < nhisto; i++){
		std::ostringstream NameString, TitleString;
		NameString << csbins[i] << "_" << direction << p
			<< "clustersize_ " << csbins[i+1];
		if(title != "") TitleString << title;
		else TitleString << "Local " + xory + " - "
		       	<< (csbins[i] + csbins[i+1]) /2 << "-" << colorrow << " clusters";
		TH1F *currentPlot = new TH1F(NameString.str().c_str(),
				TitleString.str().c_str(),nbinhisto,0,nbinhisto);
		currentPlot->UseCurrentStyle();
		currentPlot->GetYaxis()->SetTitle( "Charge sharing correction  [#mum]");
					
		currentPlot->SetLineColor(color);
		currentPlot->SetMarkerColor(color);
		for(int j = 0; j < nangleBins; j++ ){
			for(int k = 0; k < nLayerBins; k++){
				float constant = 0;
				float error = 0;
				if(direction == "phi"){
					constant = parameters.getDeltaX(j,i,k);
					error =  parameters.getErrDeltaX(j,i,k);
				}else{
					constant = parameters.getDeltaY(j,i,k);
					error =  parameters.getErrDeltaY(j,i,k);
				}
				int iBin = (nLayerBins*j)+k+1;
				currentPlot->SetBinContent(iBin,constant*1000);
				if(error == 0 && m_oneconst) error = 1e-6;
				currentPlot->SetBinError(iBin,error*1000);
			}
		}
//		std::cout << "===============================================" << std::endl;
//		std::cout << "The histogram..." << std::endl;
//		std::cout << "title: " << currentPlot->GetTitle() << std::endl;
//		std::cout << "n constants: " << currentPlot->GetNbinsX() << std::endl;
//		std::cout << "===============================================" << std::endl;
		histo->push_back(currentPlot);	
	}

	return histo;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationPlot::PlotDirection( const std::string& filename,
                                                  const std::string& direction){

	std::vector < std::vector < TH1F* > > *RefHistos = 0;
	std::vector < TH1F* > *histograms = 0;

	if(direction == "phi"){
		histograms = m_histogramsX;
		RefHistos = m_RefHistosX;
	}else{
		histograms = m_histogramsY;
		RefHistos = m_RefHistosY;
	}
	
	unsigned int nhistos_all = histograms->size();
	unsigned int nhistos = 0;
	if(!m_oneconst) nhistos = nhistos_all; // plot all histograms for shifter checks!
	else{ // only filled histos for "official" plots
		for(unsigned int i = 0; i < nhistos_all; i++){
			if((*histograms)[i]->GetMean() == 0 &&
					(*histograms)[i]->GetRMS() == 0) continue;
			nhistos++;
		}
	}
	std::vector<TCanvas*> canvases;
	for(unsigned int i = 0; i < nhistos; i++){
		TCanvas* c1 = new TCanvas();
		canvases.push_back(c1);
		float maximum = (*histograms)[i]->GetMaximum();
		for(unsigned int j = 0; j < RefHistos->size(); j++){
			float newmax = ((*RefHistos)[j])[i]->GetMaximum();
			if(maximum < newmax) maximum = newmax;
		}
		if( int((*histograms)[i]->GetNbinsX()/(2*m_nlayers)) > 20)
			c1->SetBottomMargin(0.18);
		DrawOneHisto((*histograms)[i], direction, maximum);
		DrawHistoMarkers((*histograms)[i]);
		std::string title =  filename;
		size_t found = title.find("PixelChargeInterpolationData");
		if(found != std::string::npos) title = "PixelOfflineReco" + title.substr(title.find("-"));
		if(m_oneconst) DrawLegendLatex(title.c_str(), 21 ,0.20, 0.83,
					(*histograms)[i]->GetMarkerColor(),0.05);
		else DrawTitleLatex(title.c_str(),0.15, 0.83,
					(*histograms)[i]->GetMarkerColor());
	//}

	unsigned int nreferences = RefHistos->size();
	//for(unsigned int i = 0; i < nhistos; i++){
		canvases[i]->cd();
		for(unsigned int j = 0; j < nreferences; j++){
			DrawHistoMarkers(((*RefHistos)[j])[i],(*m_referenceDrawOpt)[j],j+1);
			std::string title =  ((*RefHistos)[j])[i]->GetTitle();
			size_t found = title.find("PixelChargeInterpolationData");
			if(found != std::string::npos) title = "PixelOfflineReco" + title.substr(title.find("-"));
			if(m_oneconst) DrawLegendLatex(title.c_str(),
				       	21+5*(j+1) ,
					0.20, 0.83 - (j+1)*0.06,
					((*RefHistos)[j])[i]->GetMarkerColor(),0.05);
			else DrawTitleLatex(title.c_str(),
					0.15, 0.83 - (j+1)*0.05,
					((*RefHistos)[j])[i]->GetMarkerColor());

		}
		//DrawOneHisto((*histograms)[i], direction, maximum); 
		DrawHistoMarkers((*histograms)[i]);
	} 

	for(unsigned int i = 0; i < nhistos; i++){
		std::ostringstream name;
		name << filename << direction << i+2 << ".pdf";
		canvases[i]->Print(name.str().c_str());
	}
	
	//delete axis;
	//delete c1;
	//axis = 0;
	//c1 = 0;

}


//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationPlot::DrawOneHisto(TH1F* histo, const std::string& direction, float maximum){


	// play with the histo
	//float ymaximum = 1.5 * histo->GetMaximum();
	float ymaximum = 1.5 * maximum;
	float yminimum = - 0.1 * histo->GetMaximum();
	histo->SetMaximum(ymaximum);
	histo->SetMinimum(yminimum);
	histo->GetXaxis()->SetLabelOffset(10);
	histo->GetXaxis()->SetNdivisions(0);
	// resizing the histo!!
//	bool notfound = true;
	int xmaximum = histo->GetNbinsX();
	int xminimum = 1;
//	for(int j = xminimum; j < xmaximum && notfound; j+=2*m_nlayers){
//		for(int k = j ; k < j+2*m_nlayers; k++){
//			if(histo->GetBinContent(k) != 0){
//				xminimum = j;
//				notfound = false;
//				break;
//			}
//		}
//	}
//	notfound = true;
//	for(int j = xmaximum; j > xminimum && notfound; j-=2*m_nlayers){
//		for(int k = j ; k > j-2*m_nlayers; k--){
//			if(histo->GetBinContent(k) != 0){
//				xmaximum = j;
//				notfound = false;
//				break;
//			}
//		}
//	}
	histo->GetXaxis()->SetRange(xminimum,xmaximum);
	histo->DrawCopy("AXIS");

	DrawAxis(yminimum, ymaximum, xminimum, xmaximum, direction);
	if(!m_oneconst) DrawLayerLegend(0.65, 0.9);
	else DrawATLASLabel(0.15,0.9, 0.05,true);
	DrawTitleLatex(histo->GetTitle(), 0.73, 0.85,1,0.05);
}


//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationPlot::DrawHistoMarkers(TH1F* histo, const std::string& options, int goodj){

	int startmarker = 21;
	TH1F *markers = (TH1F *)histo->Clone();
        markers->SetMarkerStyle(startmarker+5*goodj);
	if(m_oneconst){
	       markers->Rebin(2*m_nlayers);
	       for (int j = 1; j < markers->GetNbinsX(); j++){
		       markers->SetBinContent(j,histo->GetBinContent((j-1)*2*m_nlayers +1));
	       }
	       markers->DrawCopy(options.c_str());
	}else{
		for (int j = 0; j < markers->GetNbinsX(); j++){
			int period = j%(2*m_nlayers);
			markers->SetMarkerStyle(startmarker+period);
			markers->GetXaxis()->SetRange(j+1,j+1);
			if(period == goodj || !m_oneconst)
				markers->DrawCopy(options.c_str());
		}
	}
	
	delete markers;
	markers = 0;
	
}


//////////////////////////////////////////////////////////////////////////////////////////


void PixelChargeInterpolationPlot::DrawLayerLegend(float xlegend,float ylegend){

	int startmarker = 21;
	float textsize = 0.03;
	int color = 2;
	
	std::vector<std::string> names;
	names.push_back("b-layer");
	names.push_back("Layer 1");
	names.push_back("Layer 2");
	names.push_back("Disk 1");
	names.push_back("Disk 2");
	names.push_back("Disk 3");

	for(int i = 0; i < 2*m_nlayers; i++ )
		DrawLegendLatex(names[i].c_str(), startmarker+i,
				xlegend, ylegend - i*textsize,
				color, textsize);

}


//////////////////////////////////////////////////////////////////////////////////////////


void PixelChargeInterpolationPlot::DrawAxis(float y1, float y2, float x1, float x2, const std::string& direction){
	
	float stdtextsize = 0.05;
	int stdfont = 42;
	int firstbin = int(x1/(2*m_nlayers));
	int lastbin = int(x2/(2*m_nlayers));
	int naxbins = lastbin - firstbin;
	
	std::string axtitle = "";
	double* axbins = 0;

	if(direction == "phi"){
		axbins = m_phibins;
		axtitle = "Track incident angle (#phi_{i}) [#circ]";
	}else if(direction == "eta"){
		axbins = m_etabins;
		axtitle = "Track incident pseudorapidity (#eta_{i})";
	}
  if (!axbins) { return; }

	//std::cout << naxbins << std::endl;

	TGaxis *axis = new TGaxis(x1-1, y1, x2, y1, axbins[firstbin], axbins[lastbin], naxbins);
	axis->UseCurrentStyle();
	axis->SetName(direction.c_str());
	axis->SetTitle(axtitle.c_str());
	axis->SetTitleFont(stdfont);
	axis->SetTitleSize(stdtextsize);
	float offset = 1.4;
	if(naxbins > 20) offset = 1.8;
	axis->SetTitleOffset(offset);
	std::string opt = "UNW+-";
	if(m_oneconst) opt = "UN+-";
	axis->DrawAxis(x1-1, y1, x2, y1,
			axbins[firstbin], axbins[lastbin], naxbins,opt.c_str());
	axis->DrawAxis(x1-1, y2, x2, y2,
			axbins[firstbin], axbins[lastbin], naxbins,"UN-");

	TLatex label;
	label.SetTextSize(stdtextsize);
	label.SetTextFont(stdfont);
	label.SetTextAlign(23);
	double pass = ( axis->GetX2()-axis->GetX1() ) / double( naxbins);
	double ylabel = y1 - 0.05 * (y2 - y1);
	double distance = - 0.05 * (y2 - y1);
	for(int i = 0 ; i <= naxbins; i++){
		std::ostringstream LabelString;
		LabelString << axbins[firstbin+i];
		float ylabel2 = ylabel;
		if(naxbins > 20)  ylabel2 = ylabel + i%2 * distance;
		label.DrawLatex( axis->GetX1() + i * pass, ylabel2,
				LabelString.str().c_str());
	}


}


//////////////////////////////////////////////////////////////////////////////////////////


void PixelChargeInterpolationPlot::Write(){

	for(unsigned int i = 0; i < m_histogramsX->size(); i++) 
		(*m_histogramsX)[i]->Write();
	for(unsigned int i = 0; i < m_histogramsY->size(); i++) 
		(*m_histogramsY)[i]->Write();
	       
	

}


} // end of namespace PixelCalib

#endif
