/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller.h"

using namespace std;
using namespace Monitored;

HistogramFillerFactory::HistogramFillerFactory(const ServiceHandle<ITHistSvc>& histSvc,
                                               std::string groupName)
    : m_histSvc(histSvc), m_groupName(std::move(groupName)), m_histogramCategory({
      { "EXPERT", new MonitoringGroup(m_histSvc, m_groupName, MonitoringGroup::expert) },
      { "SHIFT", new MonitoringGroup(m_histSvc, m_groupName, MonitoringGroup::shift) },
      { "DEBUG", new MonitoringGroup(m_histSvc, m_groupName, MonitoringGroup::debug) },
      { "RUNSTAT", new MonitoringGroup(m_histSvc, m_groupName, MonitoringGroup::runstat) },
      { "EXPRESS", new MonitoringGroup(m_histSvc, m_groupName, MonitoringGroup::express) },
    }) { }
    
HistogramFillerFactory::~HistogramFillerFactory() {
    for(auto monGroup : m_histogramCategory) {
        delete monGroup.second;
    }
}

HistogramFiller* HistogramFillerFactory::create(const HistogramDef& def) {
  TH1* histo(0);
  TH1* histo1D(0);
  TProfile* histoProfile(0);
  TH2* histo2D(0);
  TProfile2D* histo2DProfile(0);

  if (def.type == "TH1F") {
    histo1D = histo = create1D<TH1F>(def);
  } else if (def.type == "TH1D") {
    histo1D = histo = create1D<TH1D>(def);
  } else if (def.type == "TH1I") {
    histo1D = histo = create1D<TH1I>(def);
  } else if (def.type == "TProfile") {
    histo = create1DProfile<TProfile>(def);
    histoProfile = dynamic_cast<TProfile*>(histo);
  } else if (def.type == "TH2F") {
    histo = create2D<TH2F>(def);
    histo2D = dynamic_cast<TH2*>(histo);
  } else if (def.type == "TH2D") {
    histo = create2D<TH2D>(def);
    histo2D = dynamic_cast<TH2*>(histo);
  } else if (def.type == "TH2I") {
    histo = create2D<TH2I>(def);
    histo2D = dynamic_cast<TH2*>(histo);
  } else if (def.type == "TProfile2D") {
    histo = create2DProfile<TProfile2D>(def);
    histo2DProfile = dynamic_cast<TProfile2D*>(histo);
  }
  
  if (histo == 0) {
    throw HistogramFillerCreateException("Can not create yet histogram of type: " + def.type + "\n" +
                                         "Try one of: TH1[F,D,I], TH2[F,D,I], TProfile, TProfile2D");
  }

  setLabels(histo, def.labels);
  setOpts(histo, def.opt);

  HistogramFiller* result(0);

  if ( histo1D ){
    if ( def.opt.find("kCumulative") != std::string::npos ) {
      result = new CumulativeHistogramFiller1D(histo1D, def);
    } else if (def.opt.find("kVecUO") != std::string::npos) {
      result = new VecHistogramFiller1DWithOverflows(histo1D, def);
    } else if (def.opt.find("kVec") != std::string::npos) {
      result = new VecHistogramFiller1D(histo1D, def);
    } else {
      result = new HistogramFiller1D(histo1D, def);
    }
  } else if ( histoProfile ){
      result = new HistogramFillerProfile(histoProfile, def);
  } else if ( histo2DProfile ) {
      result = new HistogramFiller2DProfile(histo2DProfile, def);
  } else if ( histo2D ) {
      result = new HistogramFiller2D(histo2D, def);
  }
  
  return result;
}

/**
 * Create and register histogram
 * 
 * If histogram already exists under that name, re-use it
 * H      : actual type of histogram to be created
 * HBASE  : histogram base class (TH[1,2,3])
 * Args   : remaining arguments to TH constructor (except name, title)
 */
template<class H, class HBASE, typename... Types> 
HBASE* HistogramFillerFactory::create(const HistogramDef& def, Types&&... hargs) {
  // Check if histogram exists already
  HBASE* histo = m_histogramCategory[def.path]->template getHist<HBASE>(def.alias);

  if (histo) {
    return histo;
  }

  // Create the histogram and register it
  H* h = new H(def.alias.c_str(), def.title.c_str(), std::forward<Types>(hargs)...);
  StatusCode sc = m_histogramCategory[def.path]->regHist(static_cast<TH1*>(h));

  if (sc.isFailure()) {
    delete h;
    h = nullptr;
  }
  
  return h;
}

template<class H> 
TH1* HistogramFillerFactory::create1D(const HistogramDef& def) {
  return create<H,TH1>(def, def.xbins, def.xmin, def.xmax);
}

template<class H> 
TH1* HistogramFillerFactory::create1DProfile(const HistogramDef& def) {
  return create<H,TH1>(def, def.xbins, def.xmin, def.xmax, def.ymin, def.ymax);
}

template<class H> 
TH1* HistogramFillerFactory::create2D(const HistogramDef& def) {
  return create<H,TH2>(def, def.xbins, def.xmin, def.xmax, def.ybins, def.ymin, def.ymax);
}

template<class H> 
TH1* HistogramFillerFactory::create2DProfile(const HistogramDef& def) {
  return create<H,TH2>(def, def.xbins, def.xmin, def.xmax, 
                            def.ybins, def.ymin, def.ymax, def.zmin, def.zmax);
}

void HistogramFillerFactory::setOpts(TH1* hist, const string& opt) {
  // try to apply an option
  if ( opt.find("kCanRebin") != std::string::npos ) {
     hist->SetCanExtend(TH1::kAllAxes);
  } else {
     hist->SetCanExtend(TH1::kNoAxis);  
  }
  // try to apply option to make Sumw2 in histogram
  if ( opt.find("Sumw2") != std::string::npos ) {
    hist->Sumw2();
  }
}

void HistogramFillerFactory::setLabels(TH1* hist, const vector<string>& labels) {
  if (labels.empty()){
    return;
  }
  
  for ( int i = 0; i < std::min( (int)labels.size(), (int)hist->GetNbinsX() ); ++i ) {
    int bin = i+1;
    hist->GetXaxis()->SetBinLabel(bin, labels[i].c_str());
  }

  for ( int i = (int)hist->GetNbinsX(); i < std::min( (int)labels.size(), (int)hist->GetNbinsX()+(int)hist->GetNbinsY() ); ++i ) {
    int bin = i+1-(int)hist->GetNbinsX();
    hist->GetYaxis()->SetBinLabel(bin, labels[i].c_str());
  }
}

HistogramFillerFactory::MonitoringGroup::MonitoringGroup(const ServiceHandle<ITHistSvc>& histSvc, std::string groupName, Level l)
  : m_histSvc(histSvc), m_groupName(std::move(groupName)), m_level(l) { }

StatusCode HistogramFillerFactory::MonitoringGroup::regHist(TH1* h){
  return m_histSvc->regHist(level2string(m_level)+m_groupName+"/"+h->GetName(), h);
}

StatusCode HistogramFillerFactory::MonitoringGroup::deregHist(TH1* h) {
  return m_histSvc->deReg(h);
}

std::string HistogramFillerFactory::MonitoringGroup::level2string(Level l) const {
  string result = "/UNSPECIFIED_NONE_OF_EXPERT_DEBUG_SHIFT_EXPRESS_RUNSUM/";

  switch (l){
  case debug:
    result = "/DEBUG/";
    break;
  case expert:
    result = "/EXPERT/";
    break;
  case shift:
    result = "/SHIFT/";
    break;
  case express:
    result = "/EXPRESS/";
    break;
  case runsum:
    result = "/RUNSUM/";
    break;
  }

  return result;
}
  
unsigned HistogramFiller1D::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    histogram()->Fill(value);
    ++i;
  }

  return i;
} 

unsigned CumulativeHistogramFiller1D::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    unsigned bin = hist->FindBin(value);

    for (unsigned j = bin; j > 0; --j) {
      hist->AddBinContent(j);
    }

    ++i;
  }

  return i;  
}

unsigned VecHistogramFiller1D::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    hist->AddBinContent(i+1, value);
    hist->SetEntries(hist->GetEntries() + value);

    ++i;
  }

  return i;
}

unsigned VecHistogramFiller1DWithOverflows::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    hist->AddBinContent(i, value);
    hist->SetEntries(hist->GetEntries() + value);

    ++i;
  }

  return i;
}

unsigned HistogramFillerProfile::fill() {
  if (m_monVariables.size() != 2) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
  auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));

  if (valuesVector1.size() != valuesVector2.size()) {
    if (valuesVector1.size() == 1) {
      // first variable is scalar -- loop over second
      for (auto value2 : valuesVector2) {
        hist->Fill(valuesVector1[0], value2);
        ++i;
      }
    } else if (valuesVector2.size() == 1)  {
      // second varaible is scalar -- loop over first
      for (auto value1 : valuesVector1) {
        hist->Fill(value1, valuesVector2[0]); 
        ++i;
      } 
    }
  } else {
    for (i = 0; i < valuesVector1.size(); ++i) {
      hist->Fill(valuesVector1[i], valuesVector2[i]);
    }
  }
  
  return i;
}

unsigned HistogramFiller2DProfile::fill() {
  if (m_monVariables.size() != 3) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
  auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
  auto valuesVector3 = m_monVariables[2].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));
  /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
  unsigned i(0);
  if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

  }*/

  //For now lets just consider the case in which all variables are of the same length
  for (i = 0; i < valuesVector1.size(); ++i) {
    hist->Fill(valuesVector1[i], valuesVector2[i], valuesVector3[i]);
  }
  
  return i;
}

unsigned HistogramFiller2D::fill() {
  if (m_monVariables.size() != 2) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
  auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
  lock_guard<mutex> lock(*(this->m_mutex));

  if (valuesVector1.size() != valuesVector2.size()) {
    if (valuesVector1.size() == 1) {
      // first variable is scalar -- loop over second
      for (auto value2 : valuesVector2) {
        hist->Fill(valuesVector1[0], value2);
        ++i;
      }
    } else if (valuesVector2.size() == 1)  {
      // second varaible is scalar -- loop over first
      for (auto value1 : valuesVector1) {
        hist->Fill(value1, valuesVector2[0]); 
        ++i;
      } 
    }
  } else {
    for (i = 0; i < valuesVector1.size(); ++i) {
      hist->Fill(valuesVector1[i], valuesVector2[i]);
    }
  }
  
  return i;
}
