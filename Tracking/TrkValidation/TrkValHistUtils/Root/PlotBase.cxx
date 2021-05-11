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
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include <algorithm>

PlotBase::PlotBase(PlotBase *pParent, const std::string &sDir) {
  if (pParent != nullptr) {
    pParent->RegisterSubPlot(this);
  }
  std::string sParentDirectory = (pParent != nullptr) ? pParent->getDirectory() : "";
  m_sDirectory = sParentDirectory + sDir;
  m_iDetailLevel = 0;
}

void
PlotBase::initialize() {
  for (auto subNode: m_vSubNodes) {
    subNode->initialize();
  }
  initializePlots();
}

void
PlotBase::finalize() {
  for (auto subNode: m_vSubNodes) {
    subNode->finalize();
  }
  finalizePlots();
}

void
PlotBase::setDetailLevel(int iDetailLevel) {
  for (auto subNode: m_vSubNodes) {
    subNode->setDetailLevel(iDetailLevel);
  }
  m_iDetailLevel = iDetailLevel;
}

std::vector<HistData> 
PlotBase::retrieveBookedHistograms() {
  std::vector<HistData> vBookedHistograms = m_vBookedHistograms;
  for (const auto &subNode: m_vSubNodes) {
    std::vector<HistData> subNodeHists = subNode->retrieveBookedHistograms();
    vBookedHistograms.insert(vBookedHistograms.end(), subNodeHists.begin(), subNodeHists.end());
  }
  return vBookedHistograms;
}

std::vector<TreeData> 
PlotBase::retrieveBookedTrees() {
  std::vector<TreeData> vBookedTrees = m_vBookedTrees;
  for (auto subNode: m_vSubNodes) {
    std::vector<TreeData> subNodeTrees = subNode->retrieveBookedTrees();
    vBookedTrees.insert(vBookedTrees.end(), subNodeTrees.begin(), subNodeTrees.end());
  }
  return vBookedTrees;
}

std::vector<EfficiencyData> 
PlotBase::retrieveBookedEfficiencies() {
  std::vector<EfficiencyData> vBookedEfficiencies = m_vBookedEfficiencies;
  for (const auto &subNode: m_vSubNodes) {
    std::vector<EfficiencyData> subNodeHists = subNode->retrieveBookedEfficiencies();
    vBookedEfficiencies.insert(vBookedEfficiencies.end(), subNodeHists.begin(), subNodeHists.end());
  }
  return vBookedEfficiencies;
}


TH1F *
PlotBase::Book1D(const std::string &name, const std::string &labels, int nBins, float start, float end,
                 bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(false);
  TH1F *hist = new TH1F((prefix + name).c_str(), labels.c_str(), nBins, start, end);
  TH1::AddDirectory(oldstat);

  hist->Sumw2();
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TH1F *
PlotBase::Book1D(const std::string &name, TH1 *refHist, const std::string &labels, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(false);
  TH1F *hist = new TH1F((prefix + name).c_str(), labels.c_str(), refHist->GetNbinsX(),
                        refHist->GetXaxis()->GetXbins()->GetArray());
  hist->Sumw2();
  TH1::AddDirectory(oldstat);


  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TH2F *
PlotBase::Book2D(const std::string &name, const std::string &labels, int nBinsX, float startX, float endX, int nBinsY,
                 float startY, float endY, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TH2::AddDirectoryStatus();
  TH2::AddDirectory(false);
  TH2F *hist = new TH2F((prefix + name).c_str(), labels.c_str(), nBinsX, startX, endX, nBinsY, startY, endY);
  hist->Sumw2();
  TH2::AddDirectory(oldstat);


  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TH2F *
PlotBase::Book2D(const std::string &name, TH2 *refHist, const std::string &labels, bool prependDir) {
  return Book2D(name, labels, refHist->GetNbinsX(), refHist->GetXaxis()->GetXmin(), refHist->GetXaxis()->GetXmax(),
                refHist->GetNbinsY(), refHist->GetYaxis()->GetXmin(), refHist->GetYaxis()->GetXmax(), prependDir);
}

TH2F *
PlotBase::Book2D(const std::string &name, const std::string &labels, int nBinsX, Double_t *binsX, int nBinsY,
                 Double_t startY, Double_t endY, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TH2::AddDirectoryStatus();
  TH2::AddDirectory(false);
  TH2F *hist = new TH2F((prefix + name).c_str(), labels.c_str(), nBinsX, binsX, nBinsY, startY, endY);
  hist->Sumw2();
  TH2::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TH3F *
PlotBase::Book3D(const std::string &name, const std::string &labels, int nBinsX, float startX, float endX, int nBinsY,
                 float startY, float endY, int nBinsZ, float startZ, float endZ, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TH3::AddDirectoryStatus();
  TH3::AddDirectory(false);
  TH3F *hist = new TH3F((prefix + name).c_str(),
                        labels.c_str(), nBinsX, startX, endX, nBinsY, startY, endY, nBinsZ, startZ, endZ);
  hist->Sumw2();
  TH3::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TH3F *
PlotBase::Book3D(const std::string &name, TH3 *refHist, const std::string &labels, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TH3::AddDirectoryStatus();
  TH3::AddDirectory(false);
  TH3F *hist = new TH3F((prefix + name).c_str(), labels.c_str(), refHist->GetNbinsX(),
                        refHist->GetXaxis()->GetXbins()->GetArray(), refHist->GetNbinsY(),
                        refHist->GetYaxis()->GetXbins()->GetArray(), refHist->GetNbinsZ(),
                        refHist->GetZaxis()->GetXbins()->GetArray());
  TH3::AddDirectory(oldstat);

  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TProfile *
PlotBase::BookTProfile(const std::string &name, const std::string &labels, int nBinsX, float startX, float endX,
                       float startY, float endY, bool prependDir, bool useRMS) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  TProfile *hist(nullptr);
  Bool_t oldstat = TProfile::AddDirectoryStatus();
  TProfile::AddDirectory(false);
  std::string opt = useRMS ? "S" : "";
  if ((startY == -1) and (endY == -1)) {
    hist = new TProfile((prefix + name).c_str(), labels.c_str(), nBinsX, startX, endX, opt.c_str());
  } else {
    hist = new TProfile((prefix + name).c_str(), labels.c_str(), nBinsX, startX, endX, startY, endY, opt.c_str());
  }
  TProfile::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TProfile *
PlotBase::BookTProfile(const std::string &name, const std::string &labels, int nBinsX, float *binsX, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  TProfile *hist(nullptr);
  Bool_t oldstat = TProfile::AddDirectoryStatus();
  TProfile::AddDirectory(false);

  hist = new TProfile((prefix + name).c_str(), labels.c_str(), nBinsX, binsX);
  TProfile::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TProfile *
PlotBase::BookTProfileRangeY(const std::string &name, const std::string &labels, int nBinsX, double *binsX,
                             double startY, double endY, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  TProfile *hist(nullptr);
  Bool_t oldstat = TProfile::AddDirectoryStatus();
  TProfile::AddDirectory(false);

  hist = new TProfile((prefix + name).c_str(), labels.c_str(), (Int_t) nBinsX, binsX, startY, endY);
  TProfile::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TProfile2D *
PlotBase::BookTProfile2D(const std::string &name, const std::string &labels, const int nBinsX,
                         const double xlo, const double xhi, const int nBinsY, const double ylo, const double yhi,
                         bool prependDir, bool useRMS) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TProfile2D::AddDirectoryStatus();
  TProfile2D::AddDirectory(false);
  std::string opt = useRMS ? "S" : "";
  TProfile2D *hist = new TProfile2D((prefix + name).c_str(), labels.c_str(), nBinsX, xlo, xhi, nBinsY, ylo, yhi, opt.c_str());
  TProfile2D::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TProfile2D *
PlotBase::BookTProfile2D(const std::string &name, const std::string &labels, const int nBinsX, double* binsX, const int nBinsY, double* binsY, bool prependDir, bool useRMS) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  Bool_t oldstat = TProfile2D::AddDirectoryStatus();
  TProfile2D::AddDirectory(false);
  std::string opt = useRMS ? "S" : "";
  TProfile2D *hist = new TProfile2D((prefix + name).c_str(), labels.c_str(), nBinsX, binsX, nBinsY, binsY, opt.c_str());
  TProfile2D::AddDirectory(oldstat);
  m_vBookedHistograms.emplace_back(hist, m_sDirectory);
  return hist;
}

TEfficiency * 
PlotBase::BookTEfficiency(const std::string &name, const std::string & labels, const int nBinsX, const float xlo, const float xhi, const bool prependDir){
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  //Bool_t oldstat = TEfficiency::AddDirectoryStatus();
  TEfficiency *hist = new TEfficiency((prefix + name).c_str(), labels.c_str(), nBinsX, xlo, xhi);
  //hist->SetAutoSave(0);
  //hist->SetAtoFlush(0);
  hist->SetDirectory(nullptr);
  m_vBookedEfficiencies.emplace_back(hist, m_sDirectory);
  //TEfficiency::AddDirectory(oldstat);
  return hist;
}

TTree *
PlotBase::BookTree(const std::string &name, bool prependDir) {
  std::string prefix = constructPrefix(m_sDirectory, prependDir);
  TTree *tree = new TTree((prefix + name).c_str(), "");

  tree->SetAutoSave(0);
  tree->SetAutoFlush(0);
  tree->SetDirectory(nullptr);
  m_vBookedTrees.emplace_back(tree, m_sDirectory);
  return tree;
}

std::string
PlotBase::constructPrefix(std::string dir, bool prependDir) {
  if (!prependDir) {
    return "";
  }
  std::replace(dir.begin(), dir.end(), '/', '_');
  return dir;
}
