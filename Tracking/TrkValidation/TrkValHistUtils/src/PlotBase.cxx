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

#include "TrkValHistUtils/PlotBase.h"
#include "TH1D.h"
#include "TProfile.h"
#include <algorithm>

PlotBase::PlotBase(PlotBase* pParent, const std::string & sDir){
  if (pParent != 0) pParent->RegisterSubPlot(this);
  std::string sParentDirectory = (pParent != 0) ? pParent->getDirectory() : ""; 
  m_sDirectory = sParentDirectory + sDir;
  m_iDetailLevel = 0;
}

void PlotBase::initialize(){
  for (auto subNode: m_vSubNodes) subNode->initialize();
  initializePlots();
}

void PlotBase::finalize(){
  for (auto subNode: m_vSubNodes) subNode->finalize();
  finalizePlots();
}

void PlotBase::setDetailLevel(int iDetailLevel){
  for (auto subNode: m_vSubNodes) subNode->setDetailLevel(iDetailLevel);
  m_iDetailLevel = iDetailLevel;
}


std::vector<HistData> PlotBase::retrieveBookedHistograms(){
  std::vector<HistData> vBookedHistograms = m_vBookedHistograms;
  for (auto subNode: m_vSubNodes){
    std::vector<HistData> subNodeHists = subNode->retrieveBookedHistograms();
    vBookedHistograms.insert(vBookedHistograms.end(), subNodeHists.begin(), subNodeHists.end() );
  }
  return vBookedHistograms;
}


TH1* PlotBase::Book1D(const std::string & name, const std::string & labels, int nBins, float start, float end, bool prependDir){
  std::string prefix = ConstructPrefix(m_sDirectory, prependDir);
  TH1* hist = new TH1D((prefix + name).c_str(), labels.c_str(), nBins, start, end);
  hist->Sumw2();
  m_vBookedHistograms.push_back(HistData(hist,m_sDirectory));
  return hist;
}


TH1* PlotBase::Book1D(const std::string & name, TH1* refHist, const std::string & labels, bool prependDir){
  return Book1D(name, labels, refHist->GetNbinsX(), refHist->GetXaxis()->GetXmin(), refHist->GetXaxis()->GetXmax(), prependDir);
}


TH2* PlotBase::Book2D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, bool prependDir){
  std::string prefix = ConstructPrefix(m_sDirectory, prependDir);
  TH2* hist = new TH2D((prefix + name).c_str(), labels.c_str(), nBinsX, startX, endX, nBinsY, startY,endY);
  hist->Sumw2();
  m_vBookedHistograms.push_back(HistData(hist,m_sDirectory));
  return hist;
}


TH2* PlotBase::Book2D(const std::string & name, TH2* refHist, const std::string & labels, bool prependDir){
  return Book2D(name, labels, refHist->GetNbinsX(), refHist->GetXaxis()->GetXmin(), refHist->GetXaxis()->GetXmax(),
                              refHist->GetNbinsY(), refHist->GetYaxis()->GetXmin(), refHist->GetYaxis()->GetXmax(),prependDir);
}


TProfile* PlotBase::BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY, float endY, bool prependDir)
{
  std::string prefix = ConstructPrefix(m_sDirectory, prependDir);
  TProfile* hist(0);
  if (startY==-1 and endY==-1)
    hist = new TProfile((prefix+name).c_str(), labels.c_str(), nBinsX, startX, endX);
  else
    hist = new TProfile((prefix+name).c_str(), labels.c_str(), nBinsX, startX, endX, startY, endY);
  m_vBookedHistograms.push_back(HistData(hist,m_sDirectory));
  return hist;
}


TProfile* PlotBase::BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float* binsX, bool prependDir)
{
  std::string prefix = ConstructPrefix(m_sDirectory, prependDir);
  TProfile* hist(0);
  hist = new TProfile((prefix+name).c_str(), labels.c_str(), nBinsX, binsX);
  m_vBookedHistograms.push_back(HistData(hist,m_sDirectory));
  return hist;
}


std::string PlotBase::ConstructPrefix(std::string dir, bool prependDir){
  if (!prependDir) return "";
  std::replace( dir.begin(), dir.end(), '/', '_');
  return dir;
}
