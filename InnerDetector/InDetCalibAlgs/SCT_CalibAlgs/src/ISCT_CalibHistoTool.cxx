/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file ISCT_CalibHistoTool.cxx
 * Implementation file to interface class for histograms in the SCT_CalibAlgs package
 * @author Shaun Roe
 **/
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <stdexcept> // out_of_range exception

ISCT_CalibHistoTool::ISCT_CalibHistoTool():
  m_numberOfEventsHisto{0},
  m_thistSvc{nullptr},
  m_pSCTHelper{nullptr},
  m_nLb{0},
  m_nLbMerge{0} {
  //nop
}

int
ISCT_CalibHistoTool::getNumberOfEventsInBin(const int nbin) const {
  if (m_numberOfEventsHisto) return m_numberOfEventsHisto->GetBinContent(nbin);
  std::cout << "ERROR: a histogram pointer, m_numberOfEventsHisto, is zero" << std::endl;
  return 0;
}

int
ISCT_CalibHistoTool::size() const {
  return m_numberOfEventsHisto->GetEntries();
}

int
ISCT_CalibHistoTool::size(const int histogramIndex) const {
  TH1F* pHisto{nullptr};
  int result{0};
  try {
    pHisto = m_phistoVector.at(histogramIndex);
    result = pHisto->GetEntries();
  } catch (const std::out_of_range& outOfRange) {
    //do nothing, result is zero if the vector<>.at() throws
  }
  return result;
}

double
ISCT_CalibHistoTool::getBinForHistogramIndex(const int bin, const int histogramIndex) const {
  TH1F* pHisto{nullptr};
  double result{0.0};
  try {
    pHisto = m_phistoVector.at(histogramIndex);
    result = pHisto->GetBinContent(bin);
  } catch (const std::out_of_range& outOfRange) {
    //do nothing, result is zero if the vector<>.at() throws
  }
  return result;
}

void
ISCT_CalibHistoTool::binHistograms(const int nLbMerged) {
  int nLb{numberOfLb()};
  int yAxisBins{static_cast<int>(TMath::Ceil(1.0*nLb/nLbMerged))};

  for (std::vector<TH2F*>::iterator it{m_phistoVector2D.begin()}; it != m_phistoVector2D.end(); ++it) {
    (*it)->SetBins(768, -0.5, 768+0.5, yAxisBins, 0.5, nLbMerged*yAxisBins+0.5);
  }

}

void
ISCT_CalibHistoTool::setNumberOfLb(const int nLb) {
  m_nLb=nLb;
}

int
ISCT_CalibHistoTool::numberOfLb() {
  return m_nLb;
}

void
ISCT_CalibHistoTool::setLbToMerge(const int nLbMerge) {
  m_nLbMerge = nLbMerge;
}

int
ISCT_CalibHistoTool::LbToMerge() {
  return m_nLbMerge;
}
