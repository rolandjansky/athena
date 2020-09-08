/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorModules/GenAnalysis.h"
#include <cassert>
#include <cmath>



GenAnalysis::GenAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator),
    m_histSvc("THistSvc", name)
{
  declareProperty("HistKey", m_histkey="genanalysis");
  declareProperty("THistSvc", m_histSvc);
}


StatusCode GenAnalysis::initialize() {
  CHECK(GenBase::initialize());
  CHECK(m_histSvc.retrieve());
  return init();
}


StatusCode GenAnalysis::execute() {
  if (events()->empty()) {
    ATH_MSG_ERROR("No events found in McEventCollection");
    return StatusCode::FAILURE;
  }
  return analyze();
}


/////////////////////////////////////////////////////////////////////////////////


TH1D* GenAnalysis::bookHisto1D(const std::string& path, const std::string& title, const std::vector<double>& binedges,
                               const std::string& xtitle, const std::string& ytitle) {
  TH1D* h = new TH1D(path.c_str(), title.c_str(), binedges.size()-1, &binedges.front());
  h->Sumw2();
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  StatusCode st = histSvc()->regHist(fullhistopath(path), h);
  if (st.isFailure()) return 0;
  return h;
}

TH1D* GenAnalysis::bookHisto1D(const std::string& path, const std::string& title, size_t numbins, double low, double high,
                               const std::string& xtitle, const std::string& ytitle) {
  TH1D* h = new TH1D(path.c_str(), title.c_str(), numbins, low, high);
  h->Sumw2();
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  StatusCode st = histSvc()->regHist(fullhistopath(path), h);
  if (st.isFailure()) return 0;
  return h;
}


TProfile* GenAnalysis::bookProfile1D(const std::string& path, const std::string& title, const std::vector<double>& binedges,
                                     const std::string& xtitle, const std::string& ytitle) {
  TProfile* h = new TProfile(path.c_str(), title.c_str(), binedges.size()-1, &binedges.front());
  h->Sumw2();
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  StatusCode st = histSvc()->regHist(fullhistopath(path), h);
  if (st.isFailure()) return 0;
  return h;
}

TProfile* GenAnalysis::bookProfile1D(const std::string& path, const std::string& title, size_t numbins, double low, double high,
                                     const std::string& xtitle, const std::string& ytitle) {
  TProfile* h = new TProfile(path.c_str(), title.c_str(), numbins, low, high);
  h->Sumw2();
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  StatusCode st = histSvc()->regHist(fullhistopath(path), h);
  if (st.isFailure()) return 0;
  return h;
}


TH2D* GenAnalysis::bookHisto2D(const std::string& path, const std::string& title,
                               const std::vector<double>& binedgesx, const std::vector<double>& binedgesy,
                               const std::string& xtitle, const std::string& ytitle, const std::string& ztitle) {
  TH2D* h = new TH2D(path.c_str(), title.c_str(), binedgesx.size()-1, &*binedgesx.begin(), binedgesy.size()-1, &binedgesy.front());
  h->Sumw2();
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  h->SetZTitle(ztitle.c_str());
  StatusCode st = histSvc()->regHist(fullhistopath(path), h);
  if (st.isFailure()) return 0;
  return h;
}

TH2D* GenAnalysis::bookHisto2D(const std::string& path, const std::string& title,
                               size_t numbinsx, double xlow, double xhigh,
                               size_t numbinsy, double ylow, double yhigh,
                               const std::string& xtitle, const std::string& ytitle, const std::string& ztitle) {
  TH2D* h = new TH2D(path.c_str(), title.c_str(), numbinsx, xlow, xhigh, numbinsy, ylow, yhigh);
  h->Sumw2();
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  h->SetZTitle(ztitle.c_str());
  StatusCode st = histSvc()->regHist(fullhistopath(path), h);
  if (st.isFailure()) return 0;
  return h;
}



TH1* GenAnalysis::histo(std::string key) {
  TH1* h = 0;
  StatusCode st = histSvc()->getHist(fullhistopath(key), h);
  if (st.isFailure()) ATH_MSG_WARNING("No histogram with key " << key);
  return h;
}

TProfile* GenAnalysis::profile(std::string key) {
  TH1* h = 0;
  StatusCode st = histSvc()->getHist(fullhistopath(key), h);
  if (st.isFailure()) ATH_MSG_WARNING("No histogram with key " << key);
  return dynamic_cast<TProfile*>(h);
}
