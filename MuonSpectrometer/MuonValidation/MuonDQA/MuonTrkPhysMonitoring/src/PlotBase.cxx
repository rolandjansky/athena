/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// PlotBase is the base class for all plotting structures,
//
// It has an initialize function that provides a hook called
// initializePlots where histograms should be booked.
// It also provides the Booking methods (e.g. Book1D) which
// hide the implementation of the booking from the user
//
// author: Felix Socher <Felix.Socher@cern.ch>
// -------------------------------------------------------------

#include "MuonTrkPhysMonitoring/PlotBase.h"
#include <algorithm>

PlotBase::PlotBase(PlotBase* pParent){
  if (pParent != 0) pParent->RegisterSubPlot(this);
	m_sTrackCollectionName = "";
}

void PlotBase::initialize(){
	for (auto subNode: m_vSubNodes) subNode->initialize();
  initializePlots();
}

void PlotBase::finalize(){
	for (auto subNode: m_vSubNodes) subNode->finalize();
  finalizePlots();
}

void PlotBase::setTrackCollectionName(std::string sTrackCollectionName){
	for (auto subNode: m_vSubNodes) subNode->setTrackCollectionName(sTrackCollectionName);
  m_sTrackCollectionName = sTrackCollectionName;
}

std::vector<TH1F*> PlotBase::retrieveBooked1DHistograms(){
  std::vector<TH1F*> vBooked1DHistograms = m_vBooked1DHistograms;
	for (auto subNode: m_vSubNodes){
		std::vector<TH1F*> subNodeHists = subNode->retrieveBooked1DHistograms();
		vBooked1DHistograms.insert(vBooked1DHistograms.end(), subNodeHists.begin(), subNodeHists.end() );
	}
	return vBooked1DHistograms;
}

std::vector<TH2F*> PlotBase::retrieveBooked2DHistograms(){
  std::vector<TH2F*> vBooked2DHistograms = m_vBooked2DHistograms;
	for (auto subNode: m_vSubNodes){
		std::vector<TH2F*> subNodeHists = subNode->retrieveBooked2DHistograms();
		vBooked2DHistograms.insert(vBooked2DHistograms.end(), subNodeHists.begin(), subNodeHists.end() );
	}
	return vBooked2DHistograms;
}


void PlotBase::Book1D(TH1F* &hist, std::string sName, std::string sLabels, int nBinsX, float minX, float maxX){
  hist = new TH1F(sName.c_str(),sLabels.c_str(), nBinsX,minX, maxX);
	m_vBooked1DHistograms.push_back(hist);
}

void PlotBase::Book2D(TH2F* &hist, std::string sName, std::string sLabels, int nBinsX, float minX, float maxX, int nBinsY, float minY, float maxY){
  hist = new TH2F(sName.c_str(),sLabels.c_str(), nBinsX,minX,maxX,nBinsY,minY,maxY);
	m_vBooked2DHistograms.push_back(hist);
}

