/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPlotBase.cxx
 * @author shaun roe
 **/

#include "InDetPlotBase.h"
// bring Gaudi utilities in scope
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
// #include <iostream>
#include "TEfficiency.h"
// to retrieve HistogramDefinitionSvc
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include <cmath>

namespace {
  bool
  validArguments(const float arg) {
    return not (std::isnan(arg));
  }

  bool
  validArguments(const float arg, const float arg2) {
    return not (std::isnan(arg) or std::isnan(arg2));
  }

  bool
  validArguments(const float arg, const float arg2, const float arg3) {
    return not (std::isnan(arg) or std::isnan(arg2) or std::isnan(arg3));
  }
}


InDetPlotBase::InDetPlotBase(InDetPlotBase* pParent, const std::string& dirName) :
  PlotBase(pParent, dirName), m_msg("InDetPlotBase"), m_histoDefSvc(nullptr) {
  // nop
}

void
InDetPlotBase::book(TH1*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, false);
  }
  return;
}

void
InDetPlotBase::book(TProfile*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = BookTProfile(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, hd.yAxis.first,
                          hd.yAxis.second, false);
  }
  return;
}

void
InDetPlotBase::book(TH1*& pHisto, const std::string& histoIdentifier, const std::string & nameOverride, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder, nameOverride);

  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void
InDetPlotBase::book(TProfile*& pHisto, const std::string& histoIdentifier, const std::string & nameOverride, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder, nameOverride);

  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void
InDetPlotBase::book(TH2*& pHisto, const std::string& histoIdentifier, const std::string & nameOverride, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder, nameOverride);

  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void
InDetPlotBase::book(TH2*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = Book2D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, hd.nBinsY, hd.yAxis.first,
                    hd.yAxis.second, false);
  }
  return;
}

/**/
void
InDetPlotBase::book(TEfficiency*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = BookTEfficiency(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, false);
  }
  return;
}

void
InDetPlotBase::book(TEfficiency*& pHisto, const std::string& histoIdentifier, const std::string & nameOverride, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder, nameOverride);

  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void
InDetPlotBase::fillHisto(TProfile* pTprofile, const float bin, const float weight) {
  if (pTprofile and validArguments(bin, weight)) {
    pTprofile->Fill(bin, weight);
  }
}

//
void
InDetPlotBase::fillHisto(TH1* pTh1, const float value) {
  if (pTh1 and validArguments(value)) {
    pTh1->Fill(value);
  }
}

void
InDetPlotBase::fillHisto(TH1* pTh1, const float value, const float weight) {
  if (pTh1 and validArguments(value)) {
    pTh1->Fill(value, weight);
  }
}

//
void
InDetPlotBase::fillHisto(TH2* pTh2, const float xval, const float yval) {
  if (pTh2 and validArguments(xval, yval)) {
    pTh2->Fill(xval, yval);
  }
}

void
InDetPlotBase::fillHisto(TH3* pTh3, const float xval, const float yval, const float zval) {
  if (pTh3 and validArguments(xval, yval, zval)) {
    pTh3->Fill(xval, yval, zval);
  }
}

void
InDetPlotBase::fillHisto(TEfficiency* pTeff,  const float value, const bool accepted) {
  if (pTeff and validArguments(value)) {
    pTeff->Fill(accepted, value);
  }
}

/**/
SingleHistogramDefinition
InDetPlotBase::retrieveDefinition(const std::string& histoIdentifier, const std::string& folder, const std::string & nameOverride) {
  SingleHistogramDefinition s; // invalid result

  if (not m_histoDefSvc) {
    ISvcLocator* svcLoc = Gaudi::svcLocator();
    StatusCode sc = svcLoc->service("HistogramDefinitionSvc", m_histoDefSvc);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("failed to retrieve HistogramDefinitionSvc in " << __FILE__);
      throw std::runtime_error("Could initialise the HistogramDefinitionSvc");
      return s;
    }
  }
  bool folderDefault = (folder.empty() or folder == "default");
  s = m_histoDefSvc->definition(histoIdentifier, folder);
  // "default" and empty string should be equivalent
  if (folderDefault and s.empty()) {
    const std::string otherDefault = (folder.empty()) ? ("default") : "";
    s = m_histoDefSvc->definition(histoIdentifier, otherDefault);
  }
  if (s.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  if (!nameOverride.empty()){
    s.name = nameOverride; 
  }
  return s;
}
