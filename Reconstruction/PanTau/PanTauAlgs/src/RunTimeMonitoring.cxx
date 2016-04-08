/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "PanTauAlgs/RunTimeMonitoring.h"


PanTau::RunTimeMonitoring::RunTimeMonitoring(bool isActive) {
    m_IsActivated = isActive;
    m_THistSvc = 0;
}

PanTau::RunTimeMonitoring::RunTimeMonitoring( const RunTimeMonitoring& other ) {
    m_IsActivated       = other.m_IsActivated;
    m_THistSvc          = other.m_THistSvc;
}

PanTau::RunTimeMonitoring::~RunTimeMonitoring() {
    m_THistSvc = 0;
    m_HistMap.clear();
}

PanTau::RunTimeMonitoring PanTau::RunTimeMonitoring::operator=( RunTimeMonitoring rhs ) {
    m_THistSvc          = rhs.m_THistSvc;
    m_HistMap           = rhs.m_HistMap;
    m_IsActivated       = rhs.m_IsActivated;
    return *this;
}


void PanTau::RunTimeMonitoring::SetTHistSvcLink(ITHistSvc* histSvc) {
    m_THistSvc = histSvc;
}



// Histogram Creation ---------------------------------------------------------------------

void 	PanTau::RunTimeMonitoring::CreateHist1D(std::string Name, std::string xTitle, int nBins, double MinBin, double MaxBin) {
	//check if this histo already exists
    HistMap::iterator it = m_HistMap.find(Name);
    if(it != m_HistMap.end()) {
        std::cout << "PanTau-RunTimeMonitoring\t\tWARNING\tRTM:CreateHist1D:         Histogram with name " << Name << " already exists in HistMap" << std::endl;
        return;
    }
    TString HistName 	= Name;
    TString HistxTitle 	= xTitle;
    std::pair<std::string, TH1F*> toInsert = std::pair<std::string, TH1F*>(Name, new TH1F(HistName, HistName, nBins, MinBin, MaxBin));
    toInsert.second->GetXaxis()->SetTitle(HistxTitle);
    m_HistMap.insert(toInsert);
    return;
}

void PanTau::RunTimeMonitoring::CreateHist1DMap(
                                                std::vector<std::string> 	Names,
                                                std::vector<std::string> 	xTitles,
                                                std::vector<int> 			nBins,
                                                std::vector<double>		MinBins,
                                                std::vector<double>		MaxBins
                                               ) {
    //int nHists = 0;
    bool valid = false;
    if(Names.size() == xTitles.size() && Names.size() == nBins.size() && Names.size() == MinBins.size() && Names.size() == MaxBins.size()) {
        valid = true;
        //nHists = Names.size();
    }
    if(valid == false) {
        std::cout << "PanTau-RunTimeMonitoring\t\tWARNING\tRTM:CreateHist1DMap: Mismatch in sizes of input vectors" << std::endl;
        return;
    }
    for(unsigned int i=0; i<Names.size(); i++) {
        CreateHist1D(Names[i], xTitles[i], nBins[i], MinBins[i], MaxBins[i]);
    }
    return;
}



bool        PanTau::RunTimeMonitoring::RegisterHistos() {
    //loop through map and register
    HistMap::iterator itHist = m_HistMap.begin();
    HistMap::iterator itHistEnd = m_HistMap.end();
    
    for(; itHist != itHistEnd; itHist++) {
        //register the histogram with the hist service
        std::string curName = itHist->first;
        std::string SavePath = "/PanTauHists/RunTimeHistograms/";
        std::string SubFolder = "other/";
        if(curName[1] == 'G') SubFolder = "EventVariables/";
        if(curName[1] == 'T') SubFolder = "TauVariables/";
        if(curName[1] == 'E') SubFolder = "EFOVariables/";
        if(curName[1] == 'R') SubFolder = "RunTimeVariables/";
        SavePath = SavePath + SubFolder;
        SavePath = SavePath + curName;
        StatusCode scRegHist = m_THistSvc->regHist(SavePath.c_str(), itHist->second);
        if( scRegHist.isFailure() ) {
            std::cout << "PanTau-RunTimeMonitoring\t\tERROR\tRegistration of histo " << curName << " failed" << std::endl;
            return false;
        }
    }
    return true;
}



// Histogram Filling -----------------------------------------------------------------------

void	PanTau::RunTimeMonitoring::FillHisto(std::string itsName, double fillValue) {
    if(m_IsActivated == false) return;

    //search histogram called itsName in HistMap and fill fillValue into it if it is found
    HistMap::iterator itHist = m_HistMap.find(itsName);
    if(itHist == m_HistMap.end()) return;
    
    itHist->second->Fill(fillValue);
    return;
}
