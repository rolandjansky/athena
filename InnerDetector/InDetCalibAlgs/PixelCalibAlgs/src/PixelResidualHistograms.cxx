/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelResidualHistograms_C
#define PixelResidualHistograms_C

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

#include <TH1.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TDirectory.h>

#include "PixelCalibAlgs/PixelResidualHistograms.h"
#include "PixelCalibAlgs/MultiHisto.icc"
#include "PixelCalibAlgs/AtlasStyle.h"

namespace PixelCalib{

PixelResidualHistograms::PixelResidualHistograms(std::string name,
				       std::string title,
				       double limits,
				       int nbins,
				       const std::vector< std::vector < float > > &binnage,
				       const std::vector< std::string > &binnames):
	m_HistogramsVector(0),
	m_MeanProfilesVector(0),
	m_RMSProfilesVector(0),
	m_GlobalHisto(0),
	m_binnames(binnames),
	m_binnage(binnage),
	m_axisName(""){

	TH1D *Histomodel = new TH1D(name.c_str(),title.c_str(),nbins,-limits,limits);
	m_HistogramsVector =  new MultiHisto<TH1D>(*Histomodel,m_binnames,m_binnage);

	SetAxisTitle(title);

	m_MeanProfilesVector = new std::vector<TProfile*>(m_binnames.size());
	m_RMSProfilesVector = new std::vector<TProfile*>(m_binnames.size());

	delete Histomodel;
	Histomodel = 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

PixelResidualHistograms::~PixelResidualHistograms(){

	delete m_HistogramsVector;
	delete m_MeanProfilesVector;
	delete m_RMSProfilesVector;

	m_HistogramsVector = 0;
	m_MeanProfilesVector = 0;
	m_RMSProfilesVector = 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelResidualHistograms::Fill(float residual, std::vector< float > parameters){
	m_HistogramsVector->Fill(residual,1,parameters);
}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelResidualHistograms::SetAxisTitle(std::string title){

	m_axisName = title;
	for(unsigned int i = 0 ; i < m_HistogramsVector->GetNhistos() ; i++)
		m_HistogramsVector->GetHisto(i)->GetXaxis()->SetTitle(title.c_str());


}

//////////////////////////////////////////////////////////////////////////////////////////

int PixelResidualHistograms::Write(bool writebins){

	TDirectory *current = gDirectory;
	current->mkdir( m_HistogramsVector->GetName() )->cd();

	int writtenhistos = 0;
	m_HistogramsVector->Write();
	writtenhistos +=  m_HistogramsVector->GetNhistos();

	TProfile *swap = 0;
	for(unsigned int i = 0; i < m_MeanProfilesVector->size(); i++){
		swap = GetProfile(i,true,writebins);
		if( swap->Write() ) writtenhistos++;
		swap = GetProfile(i,false,writebins);
		if( swap->Write() ) writtenhistos++;
	}

	TH1D *swap2 = GetGlobalHisto();
	if( swap2->Write() ) writtenhistos++;

	current->cd();	
	swap = 0;
	swap2 = 0;
	return writtenhistos;	
}

//////////////////////////////////////////////////////////////////////////////////////////

int PixelResidualHistograms::Read(){


	TDirectory *current = gDirectory;
	TDirectory *globaldir = (TDirectory *)current->Get(m_HistogramsVector->GetName());
	globaldir->cd();
	
	globaldir = (TDirectory *)gDirectory->Get(m_HistogramsVector->GetName());
	
	int readhistos = 0;
	readhistos += m_HistogramsVector->FillFromFile(globaldir);
	
	current->cd();
	return readhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

TH1D* PixelResidualHistograms::GetHisto(int i) const{
	return m_HistogramsVector->GetHisto(i);
}

//////////////////////////////////////////////////////////////////////////////////////////

TProfile* PixelResidualHistograms::GetMeanProfile(const std::string binname){

	for(unsigned int i = 0; i < m_binnames.size(); i++)
		if(m_binnames[i] == binname) return GetProfile(i,false);
	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

TProfile* PixelResidualHistograms::GetRMSProfile(const std::string binname){

	for(unsigned int i = 0; i < m_binnames.size(); i++)
		if(m_binnames[i] == binname) return GetProfile(i,true);
	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

TH1D* PixelResidualHistograms::GetGlobalHisto(){


	if( m_GlobalHisto != 0) return m_GlobalHisto;

	TH1 *swap = m_HistogramsVector->GetHisto(0);
	double limits = swap->GetXaxis()->GetXmax();
	int nbins = swap->GetNbinsX();
	std::string name = m_HistogramsVector->GetName() + std::string("_global");
	std::string title = m_HistogramsVector->GetTitle();

	m_GlobalHisto = new TH1D(name.c_str(), title.c_str(),
				nbins,-limits,limits);
	m_GlobalHisto->GetXaxis()->SetTitle(title.c_str());

	for(unsigned int i = 0 ; i < m_HistogramsVector->GetNhistos(); i++){
		swap = GetHisto(i);
		m_GlobalHisto->Add(swap);
	}

	return m_GlobalHisto;

}

//////////////////////////////////////////////////////////////////////////////////////////

TProfile* PixelResidualHistograms::GetProfile(const int binnumber, bool RMS, bool savebis){

	// check if it has already been calculated!
	if(RMS && (*m_RMSProfilesVector)[binnumber] != 0)
		return (*m_RMSProfilesVector)[binnumber];
	else if( (*m_MeanProfilesVector)[binnumber] != 0)
		return (*m_MeanProfilesVector)[binnumber];

	// Allocate the new TProfile
	int nProfileBins = ( m_binnage[binnumber] ).size() - 1;
	float* xbins = new float[nProfileBins+1];
	std::string Xvar_name = m_binnames[binnumber];
	std::string Xvar_name_s = Xvar_name;
	size_t found0 = Xvar_name_s.find("#");
	if(found0 != std::string::npos) Xvar_name_s = Xvar_name_s.substr(found0+1);
	size_t found1 = Xvar_name_s.find("_");
	if(found1 != std::string::npos)	Xvar_name_s = Xvar_name_s.substr(0,found1);
	for(int i = 0 ; i < nProfileBins+1 ; i++)
		xbins[i] = (m_binnage[binnumber])[i];
	std::string name = std::string(m_HistogramsVector->GetName());
	std::string title = std::string(m_HistogramsVector->GetTitle());
	std::string RMSname = name + std::string("_RMS_vs_") + Xvar_name_s;
	std::string RMStitle = std::string("Resolution of ") +
			title + std::string(" vs ") + Xvar_name;
	name = name + std::string("_Mean_vs_") + Xvar_name_s;
	title = std::string("Mean of ") + title + std::string(" vs ") + Xvar_name;

	TProfile  *theMeanProfile = new TProfile( name.c_str(), title.c_str(),
			nProfileBins, xbins);
	TProfile  *theRMSProfile = new TProfile( RMSname.c_str(), RMStitle.c_str(),
			nProfileBins, xbins);

	theMeanProfile->GetXaxis()->SetTitle( Xvar_name.c_str() );
	theRMSProfile->GetXaxis()->SetTitle( Xvar_name.c_str() );
	theMeanProfile->GetYaxis()->SetTitle( (std::string("Mean of ") + m_axisName).c_str() );
	theRMSProfile->GetYaxis()->SetTitle( (std::string("RMS of ") + m_axisName).c_str() );

	// fill it bin by bin
	std::vector<int> indexes;
	for(int ibin = 0; ibin < nProfileBins; ibin++){
		// creating the histogram for this bin..
		std::string bintitle = m_HistogramsVector->GetTitle();
		std::ostringstream binname;
		binname << m_HistogramsVector->GetName() << "vs" << Xvar_name_s << "_" << ibin;
		TH1 *swap = m_HistogramsVector->GetHisto(0);
		double limits = swap->GetXaxis()->GetXmax();
		int nbins = swap->GetNbinsX();
		TH1D *bin = new TH1D(binname.str().c_str(), bintitle.c_str(),
				nbins,-limits,limits);
		for(unsigned int i = 0 ; i < m_HistogramsVector->GetNhistos(); i++){
			indexes = m_HistogramsVector->GetDivisionsIndexes(i);
			if(indexes[binnumber] == ibin){
				swap = GetHisto(i);
				bin->Add(swap);
			}
		}

		double entries = 0.;
		double mean = 0.;
		double mean_error = 0.;
		double rms = 0.;
		double rms_error = 0.;

		if(bin->GetEntries() > 50){
			GetCoreParameters(bin,mean,mean_error,rms,rms_error,entries);
		}

		if(entries > 1){
			rms_error = sqrt( fabs(rms_error*rms_error*entries*entries 
						- rms*rms/entries));
			mean_error = sqrt( fabs(mean_error*mean_error*entries*entries
						+ mean*mean/entries));
		}
					
		theMeanProfile->SetBinContent(ibin+1,mean*entries);
		theMeanProfile->SetBinError(ibin+1,mean_error);
		theMeanProfile->SetBinEntries(ibin+1,entries);
		theRMSProfile->SetBinContent(ibin+1,rms*entries);
		theRMSProfile->SetBinError(ibin+1,rms_error);
		theRMSProfile->SetBinEntries(ibin+1,entries);
		
		//std::cout << theMeanProfile->GetBinContent(ibin+1) << "  "
		//<< theMeanProfile->GetBinError(ibin+1) << std::endl;

		if(entries > 0 && savebis) bin->Write();
		delete bin;
		bin = 0;
		swap = 0;

	}

	// put the calcuated profile in he correct place and return it!
	(*m_MeanProfilesVector)[binnumber] = theMeanProfile;
	(*m_RMSProfilesVector)[binnumber] = theRMSProfile;
	if(RMS) return theRMSProfile;
	else return theMeanProfile;

}

//////////////////////////////////////////////////////////////////////////////////////////

std::vector <TCanvas*> *PixelResidualHistograms::DrawProfiles(int color, int marker, float labely,
		std::vector <TCanvas*> *canvasvector, std::string name ){

	SetAtlasStyle();	
	std::string drawoptions = "Psame";
	std::string legend = name;
	if(canvasvector == 0){
		drawoptions = "P";
		canvasvector = new std::vector<TCanvas*>();
		for(unsigned int i = 0; i < 2 * m_MeanProfilesVector->size(); i++){
			TCanvas *c1 = new TCanvas();
			c1->UseCurrentStyle();
			canvasvector->push_back(c1);
		}
	}else if( canvasvector->size() != 2 * m_MeanProfilesVector->size() ) return 0;


	TProfile *swap = 0;
	for(unsigned int i = 0; i < m_MeanProfilesVector->size(); i++){
		// mean profile!
		(*canvasvector)[i]->cd();
		swap = GetProfile(i,false);
		swap->UseCurrentStyle();
		swap->SetLineColor(color);
		swap->SetMarkerColor(color);
		swap->SetMarkerStyle(marker);
		swap->Draw(drawoptions.c_str());
		swap->GetYaxis()->SetTitleOffset(1.2);
		double maximum = swap->GetMaximum();
		double minimum = swap->GetMinimum();
		swap->SetMaximum(maximum + fabs(minimum) + fabs(maximum));
		swap->SetMinimum(minimum - fabs(maximum) - fabs(minimum));
		

		if(drawoptions == "P"){
			size_t found = std::string(swap->GetTitle()).find("p_{T}");
			if(found != std::string::npos) (*canvasvector)[i]->SetLogx();
			(*canvasvector)[i]->SetGridy();
			DrawTitleLatex(swap->GetTitle(), 0.2,0.87);
			(*canvasvector)[i]->SetName(swap->GetName());
		}
		if(labely != 0){
			if(legend == "") legend = swap->GetTitle();
			DrawLegendLatex(legend.c_str(),marker,0.7,labely,color,0.04);
		}

		// rms profile!
		int i2 = i+m_MeanProfilesVector->size();
		(*canvasvector)[i2]->cd();
		swap = GetProfile(i,true);
		swap->UseCurrentStyle();
		swap->SetLineColor(color);
		swap->SetMarkerColor(color);
		swap->SetMarkerStyle(marker);
		swap->Draw(drawoptions.c_str());
		swap->GetYaxis()->SetTitleOffset(1.2);
		swap->SetMaximum(2*fabs(swap->GetMaximum()));
		

		if(drawoptions == "P"){
			size_t found = std::string(swap->GetTitle()).find("p_{T}");
			if(found != std::string::npos) (*canvasvector)[i2]->SetLogx();
			(*canvasvector)[i2]->SetGridy();
			DrawTitleLatex(swap->GetTitle(), 0.2,0.87);
			(*canvasvector)[i2]->SetName(swap->GetName());
		}
		if(labely != 0){
			if(legend == "") legend = swap->GetTitle();
			DrawLegendLatex(legend.c_str(),marker,0.7,labely,color,0.04);
		}

	}

	swap = 0;
	return canvasvector;
}


//////////////////////////////////////////////////////////////////////////////////////////
/*
const std::vector< std::string > *PixelResidualHistograms::GetBinsNames() const{
	return &m_binnames;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
const std::vector < float > *PixelResidualHistograms::GetBins(std::string binname) const{
	
	for(unsigned int i = 0; i < m_binnames.size(); i++)
		if(m_binnames[i] == binname) return &(m_binnage[i]);
	return 0;	

}
*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
PixelResidualHistograms *PixelResidualHistograms::Clone() const{

	const char *title = m_HistogramsVector->GetTitle();
	TH1 *swap = m_HistogramsVector->GetHisto(0);
	double limit = swap->GetBinLowEdge(1);
	PixelResidualHistograms *theclone = 
		new PixelResidualHistograms(m_GlobalName,std::string(title),
							-limit,m_binnage,m_binnames);
	return theclone;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
int PixelResidualHistograms::Analyze(PixelResidualHistograms *reference,
		std::string referencename){

	SetAtlasStyle();
	TCanvas *c1 = new TCanvas();
	c1->UseCurrentStyle();
	
	int nhistos =  m_HistogramsVector->GetNhistos();

	char *currpath = getcwd(nullptr,0);
        mkdir(m_GlobalName.c_str(),S_IRWXU | S_IRWXG | S_IRWXO);
	chdir(m_GlobalName.c_str());

	for(int i = 0 ; i < nhistos ; i++){
		TH1D *swap = m_HistogramsVector->GetHisto(i);
		if( swap->GetEntries() < 100) continue;
		DrawHisto(i,4,27,0.73,0.75,std::string("Analog"),std::string("PE"));
		if(reference != 0 ) reference->DrawHisto(i,2,22,0.73,0.6,
							referencename,std::string("samePE"));
		DrawATLASLabel(0.65,0.87);
		DrawTitleLatex(swap->GetTitle(), 0.2,0.87);
		std::string name = std::string(swap->GetName()) + std::string(".pdf");
		c1->Print(name.c_str());
	}

	if(GetProfile() != 0){
		DrawProfile(4,27,0.2,0.75,std::string("Analog"),std::string("PE"));
		if(reference != 0 )
			reference->DrawProfile(2,22,0.2,0.6,
					referencename,std::string("samePE"));
		DrawTitleLatex(m_HistogramsVector->GetTitle(), 0.2,0.87);
		std::string name = std::string(m_HistogramsVector->GetName()) +
								std::string("Profile.pdf");
		c1->Print(name.c_str());
	}

	chdir(currpath);
	delete currpath;
	delete c1;

	return nhistos;

}
*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
void PixelResidualHistograms::DrawHisto(int iHisto,
		int color, int marker, float labelx, float labely,
		std::string name,std::string Options){

	TH1D *swap = m_HistogramsVector->GetHisto(iHisto);
	swap->UseCurrentStyle();
	swap->SetLineColor(color);
	swap->SetMarkerColor(color);
	swap->SetMarkerStyle(marker);
	swap->Sumw2();
	swap->Scale(1/swap->GetEntries());
	swap->GetXaxis()->SetTitle("Residuals (#mum)");
	swap->GetYaxis()->SetTitle("Fraction of pixel clusters");
	swap->GetYaxis()->SetTitleOffset(1.2);
	
	double entries = 0.;
	double mean = 0.;
	double mean_err = 0.;
	double rms = 0.;
	double rms_err = 0.;
	GetCoreParameters(swap,mean,mean_err,rms,rms_err,entries);

	swap->DrawCopy(Options.c_str());

	std::ostringstream LegendString;
	LegendString.flags(std::ios::fixed);
	LegendString.precision(2);
	LegendString << name << " Position: - ";
	LegendString << "Mean: " <<  mean << " #pm " << mean_err << " #mum - ";
	LegendString << "RMS: " << rms << " #pm " << rms_err << " #mum";
	DrawLegendLatex(LegendString.str().c_str(), marker, labelx, labely, color);

}

*/
//////////////////////////////////////////////////////////////////////////////////////////
/*
void PixelResidualHistograms::DrawProfile(int color, int marker, float labelx, float labely,
					std::string name,std::string Options){

	TProfile  *swap = GetProfile( name + std::string("Profile") );
	if(swap == 0) return;
	swap->UseCurrentStyle();
	swap->SetLineColor(color);
	swap->SetMarkerColor(color);
	swap->SetMarkerStyle(marker);
	swap->GetXaxis()->SetTitle((m_binnames[0]).c_str());
	swap->GetYaxis()->SetTitle("RMS of residuals (#mum)");
	//swap->GetYaxis()->SetTitleOffset(1.2);
	swap->DrawCopy(Options.c_str());
	std:: string NameString = name + std::string(" Position");
	DrawLegendLatex(NameString.c_str(), marker, labelx, labely, color);

}

*/
} // end namespace

#endif // #ifdef PixelResidualHistograms_C
