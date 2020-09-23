/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelClusterOnTrackErrorPlot_cxx
#define PixelClusterOnTrackErrorPlot_cxx

#include <vector>
#include <string>
#include <sstream>

#include <TH1.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TGaxis.h>

#include "PixelCalibAlgs/PixelClusterOnTrackErrorPlot.h"
#include "PixelConditionsData/PixelClusterOnTrackErrorData.h"
#include "PixelCalibAlgs/AtlasStyle.h"

namespace PixelCalib{

PixelClusterOnTrackErrorPlot::PixelClusterOnTrackErrorPlot(
		PixelClusterOnTrackErrorData &parameters, bool oneconst):
	m_histogramsX(0), m_histogramsY(0),
	m_RefHistosX(0), m_RefHistosY(0),
	m_etabins(0), m_phibins(0), m_netabins(0), m_nphibins(0),
	m_referenceDrawOpt(0), m_oneconst(oneconst){

//	std::cout << "===============================================" << std::endl;
//	std::cout << "What we get to plot..." << std::endl;
//	std::cout << "x constants: " << parameters.getNumberOfPhiBarrelBins() << std::endl;
//	std::cout << "x constants: " << parameters.getIncidenceAngleBins().size() << " "
//		<< parameters.getClusterSizeBinsX().size() << std::endl;
//	for(unsigned int i = 0; i <  parameters.getIncidenceAngleBins().size(); i++){
//		std::cout << (parameters.getIncidenceAngleBins())[i] << std::endl;
//	}
//	std::cout << "y constants: " << parameters.getNumberOfEtaBarrelBins() << std::endl;
//	std::cout << "y constants: " << parameters.getEtaBins().size() << " "
//		<< parameters.getClusterSizeBinsY().size() << std::endl;
//	std::cout << "===============================================" << std::endl;

		
	SetAtlasStyle();
	m_referenceDrawOpt =  new std::vector<std::string>();
	m_RefHistosX = new std::vector < std::vector < TH1F* > >();
	m_RefHistosY = new std::vector < std::vector < TH1F* > >();
	m_histogramsX = HistogramsFromConstants(parameters,"phi",2);
	m_histogramsY = HistogramsFromConstants(parameters,"eta",2);

}

//////////////////////////////////////////////////////////////////////////////////////////

PixelClusterOnTrackErrorPlot::~PixelClusterOnTrackErrorPlot(){

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

void PixelClusterOnTrackErrorPlot::AddReference(
		PixelClusterOnTrackErrorData &parameters,
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
	
void PixelClusterOnTrackErrorPlot::Plot(const std::string& output){


	PlotDirection(output, "phi");
	PlotDirection(output, "eta");
	
}
	
//////////////////////////////////////////////////////////////////////////////////////////

std::vector < TH1F* > *PixelClusterOnTrackErrorPlot::HistogramsFromConstants(
			PixelClusterOnTrackErrorData &parameters,
			const std::string& direction, int color, const std::string& title){

	std::vector <TH1F*> *histo = new std::vector <TH1F*>();
	std::vector<float> anglebins;
	std::vector<float> csbins;
	int nConstants = 0;
	std::string xory, colorrow;

	if(direction == "phi"){
		anglebins = parameters.getIncidenceAngleBins();
		csbins = parameters.getClusterSizeBinsX();
		nConstants = parameters.getNumberOfPhiBarrelBins();
		if(m_phibins == 0 && m_nphibins == 0){
			m_nphibins = anglebins.size()+1;
			m_phibins = new double[m_nphibins];
			for(int i = 0; i < m_nphibins-1; i++)
				m_phibins[i] = anglebins[i];
			m_phibins[m_nphibins-1] = 180;
		}
		xory = "x";
		colorrow = "row"; 

	}else{
		anglebins = parameters.getEtaBins();
		csbins = parameters.getClusterSizeBinsY();
		nConstants = parameters.getNumberOfEtaBarrelBins();
		if(m_etabins == 0 && m_netabins == 0){
			m_netabins = anglebins.size()+1;
			m_etabins = new double[m_netabins];
			for(int i = 0; i < m_netabins-1; i++)
				m_etabins[i] = anglebins[i];
			m_etabins[m_netabins-1] = 2.5;
		}
		xory = "y";
		colorrow = "col";
	}
	
	int nangleBins = anglebins.size();
	int nCsBins = csbins.size();
	int nLayerBins = nConstants / (nangleBins * nCsBins);
	int nhisto = nCsBins;
	int nbinhisto = nLayerBins * nangleBins;

//	std::cout << "===============================================" << std::endl;
//	std::cout <<  "PixelClusterOnTrackErrorPlot" << std::endl;
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
			<< "clustersize_ " << csbins[i]+1;
		if(title != "") TitleString << title;
		else TitleString << "Local " + xory + " - "
		       	<< (csbins[i] + csbins[i]+1) /2 << "-" << colorrow << " clusters";
		TH1F *currentPlot = new TH1F(NameString.str().c_str(),
				TitleString.str().c_str(),nbinhisto,0,nbinhisto);
		currentPlot->UseCurrentStyle();
		currentPlot->GetYaxis()->SetTitle( "Cluster error [#mum]");
					
		currentPlot->SetLineColor(color);
		currentPlot->SetMarkerColor(color);
		for(int j = 0; j < nangleBins; j++ ){
			for(int k = 0; k < nLayerBins; k++){
				float constant = 0;
				if(direction == "phi"){
					double angle = anglebins[j]+1;
					if(j+1 < nangleBins) angle = (anglebins[j] + anglebins[j+1])/2;
					int index = parameters.getBarrelBinPhi(angle,i+1);
					constant = parameters.getPixelBarrelPhiError(index);
					//std::cout << "angle: " << angle << " | csize bin: " << i+1 << std::endl;
					//std::cout << "---> const index: " << index << " | const val: " << constant << std::endl;
				}else{
					double angle = anglebins[j]+1;
					if(j+1 < nangleBins) angle = (anglebins[j] + anglebins[j+1])/2;
					int index = parameters.getBarrelBinEta(angle,i+1,k+1);
					constant = parameters.getPixelBarrelEtaError(index);
					//std::cout << "angle: " << angle << " | csize y bin: " << i+1 << " | csize x bin: " << k+1 << std::endl;
					//std::cout << "---> const index: " << index << " | const val: " << constant << std::endl;
				}
				int iBin = (nLayerBins*j)+k+1;
				currentPlot->SetBinContent(iBin,constant*1000);
			}
		}
		//std::cout << "===============================================" << std::endl;
		//std::cout << "The histogram..." << std::endl;
		//std::cout << "title: " << currentPlot->GetTitle() << std::endl;
		//std::cout << "n constants: " << currentPlot->GetNbinsX() << std::endl;
		//std::cout << "===============================================" << std::endl;
		histo->push_back(currentPlot);	
	}

	return histo;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelClusterOnTrackErrorPlot::PlotDirection( const std::string& filename,
                                                  const std::string& direction){

	std::vector < std::vector < TH1F* > > *RefHistos = 0;
	std::vector < TH1F* > *histograms = 0;
	int divider = 0; 

	if(direction == "phi"){
		histograms = m_histogramsX;
		RefHistos = m_RefHistosX;
		divider = 1;
	}else{
		histograms = m_histogramsY;
		RefHistos = m_RefHistosY;
		divider = m_histogramsX->size();
	}
	
	unsigned int nhistos_all = histograms->size();
	unsigned int nhistos = 0;
	for(unsigned int i = 0; i < nhistos_all; i++){
		if((*histograms)[i]->GetMean() == 0 &&
				(*histograms)[i]->GetRMS() == 0) continue;
		nhistos++;
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
		DrawOneHisto((*histograms)[i], direction, maximum);
		DrawHistoMarkers((*histograms)[i],divider);
//		if(m_oneconst) DrawLegendLatex("Current constants", 21 ,0.43, 0.90,
//					(*histograms)[i]->GetMarkerColor(),0.04);
		DrawTitleLatex("Current constants",0.15, 0.85,
					(*histograms)[i]->GetMarkerColor());
	}

	unsigned int nreferences = RefHistos->size();
	for(unsigned int j = 0; j < nreferences; j++){
		for(unsigned int i = 0; i < nhistos; i++){
			canvases[i]->cd();
			DrawHistoMarkers(((*RefHistos)[j])[i],divider,(*m_referenceDrawOpt)[j],j+1);
//			if(m_oneconst) DrawLegendLatex(((*RefHistos)[j])[i]->GetTitle(),
//				       	21+ j+1 ,
//					0.43, 0.90 - (j+1)*0.05,
//					((*RefHistos)[j])[i]->GetMarkerColor(),0.04);
			DrawTitleLatex(((*RefHistos)[j])[i]->GetTitle(),
					0.15, 0.85 - (j+1)*0.05,
					((*RefHistos)[j])[i]->GetMarkerColor());

		}
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

void PixelClusterOnTrackErrorPlot::DrawOneHisto(TH1F* histo, const std::string& direction, float maximum){


	// play with the histo
	//float ymaximum = 1.5 * histo->GetMaximum();
	float ymaximum = 1.5 * maximum;
	float yminimum = - 0.1 * histo->GetMaximum();
	histo->SetMaximum(ymaximum);
	histo->SetMinimum(yminimum);
	histo->GetXaxis()->SetLabelOffset(10);
	histo->GetXaxis()->SetNdivisions(0);
	// resizing the histo!!
//      bool notfound = true;
	int xmaximum = histo->GetNbinsX();
	int xminimum = 0;
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

	//std::cout << direction << " axis limits: " << xminimum << " -- " << xmaximum << std::endl;
	DrawAxis(yminimum, ymaximum, xminimum, xmaximum, direction);
	if(!m_oneconst && direction == "eta") DrawLayerLegend(0.50, 0.9);
	DrawTitleLatex(histo->GetTitle(), 0.75, 0.85);
	DrawATLASLabel(0.15,0.9);
}


//////////////////////////////////////////////////////////////////////////////////////////

void PixelClusterOnTrackErrorPlot::DrawHistoMarkers(TH1F* histo, int divider, const std::string& options, int goodj){

	int startmarker = 21;
	TH1F *markers = (TH1F *)histo->Clone();
	for (int j = 0; j < markers->GetNbinsX(); j++){
		int period = j%(divider);
		markers->SetMarkerStyle(startmarker+period);
		markers->GetXaxis()->SetRange(j+1,j+1);
		if(period == goodj || !m_oneconst)
		       	markers->DrawCopy(options.c_str());
	}
	
	delete markers;
	markers = 0;
	
}


//////////////////////////////////////////////////////////////////////////////////////////


void PixelClusterOnTrackErrorPlot::DrawLayerLegend(float xlegend,float ylegend){

	int startmarker = 21;
	float textsize = 0.03;
	int color = 2;
	
	for(unsigned int i = 0; i < m_histogramsX->size(); i++ ){
		std::ostringstream LabelString;
		LabelString << i+1 << "-row clusters";
		DrawLegendLatex(LabelString.str().c_str(), startmarker+i,
				xlegend, ylegend - i*textsize,
				color, textsize);
	}

}


//////////////////////////////////////////////////////////////////////////////////////////


void PixelClusterOnTrackErrorPlot::DrawAxis(float y1, float y2, float x1, float x2, const std::string& direction){
	
	std::string axtitle = "";
	double* axbins = 0;
	int divider = 1;

	if(direction == "phi"){
		axbins = m_phibins;
		axtitle = "#phi_{i} [#circ]";
	}else if(direction == "eta"){
		axbins = m_etabins;
		axtitle = "#eta_{i}";
		divider = m_histogramsX->size();
	}
  if (!axbins) { return; }

	float stdtextsize = 0.05;
	int stdfont = 42;

	int firstbin = int(x1/(divider));
	int lastbin = int(x2/(divider));
	int naxbins = lastbin - firstbin;


	//std::cout << direction << " axis limits: " << x1 << " -- " << x2 << std::endl;
	//std::cout << direction << " axis limits: " << firstbin << " -- " << lastbin << std::endl;
	//std::cout << "n bins on axis: " << naxbins << std::endl;

	TGaxis *axis = new TGaxis(x1, y1, x2, y1,
			axbins[firstbin], axbins[lastbin], naxbins, "UNW+-");
	axis->UseCurrentStyle();
	axis->SetName(direction.c_str());
	axis->SetTitle(axtitle.c_str());
	axis->SetTitleFont(stdfont);
	axis->SetTitleSize(stdtextsize);
	axis->SetTitleOffset(1.4);
	axis->Draw();

	TLatex label;
	label.SetTextSize(0.035);
	label.SetTextAlign(23);
	double pass = ( axis->GetX2()-axis->GetX1() ) / double( naxbins);
	double ylabel = y1 - 0.05 * (y2 - y1);
	for(int i = 0 ; i <= naxbins; i++){
		std::ostringstream LabelString;
		LabelString << axbins[firstbin+i];
		//std::cout << LabelString.str() << std::endl;
		label.DrawLatex( axis->GetX1() + i * pass, ylabel,
				LabelString.str().c_str());
	}


}


//////////////////////////////////////////////////////////////////////////////////////////


void PixelClusterOnTrackErrorPlot::Write(){

	for(unsigned int i = 0; i < m_histogramsX->size(); i++) 
		(*m_histogramsX)[i]->Write();
	for(unsigned int i = 0; i < m_histogramsY->size(); i++) 
		(*m_histogramsY)[i]->Write();
	       
	

}


} // end of namespace PixelCalib

#endif
