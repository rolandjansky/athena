/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TCCPlotsBase.h"

// bring Gaudi utilities in scope
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"

// to retrieve HistogramDefinitionSvc
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"

#include <cmath>

using CLHEP::GeV;

namespace {
  bool
  validArguments(const float arg) {
    return not (std::isnan(arg));
  }

  bool
  validArguments(const float arg1, const float arg2) {
    return not (std::isnan(arg1) or std::isnan(arg2));
  }
  
  bool
  validArguments(const float arg1, const float arg2, const float arg3) {
    return not (std::isnan(arg1) or std::isnan(arg2) or std::isnan(arg3));
  }
}

TCCPlotsBase::TCCPlotsBase(PlotBase* pParent, std::string folder):
  PlotBase(pParent, folder), 
  m_folder(folder),
  m_histoDefSvc(nullptr) {}  
  

void TCCPlotsBase::book(TH1*& pHisto, const std::string& histoIdentifier, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder);
  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void TCCPlotsBase::book(TH1*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = Book1D(hd.name, hd.allTitles, 
		    hd.nBinsX, hd.xAxis.first, hd.xAxis.second, 
		    false);
  }
  return;
}

void TCCPlotsBase::book(TH2*& pHisto, const std::string& histoIdentifier, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder);
  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void TCCPlotsBase::book(TH2*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = Book2D(hd.name, hd.allTitles, 
		    hd.nBinsX, hd.xAxis.first, hd.xAxis.second, 
		    hd.nBinsY, hd.yAxis.first, hd.yAxis.second, 
		    false);
  }
  return;
}

void TCCPlotsBase::book(TH3*& pHisto, const std::string& histoIdentifier, const std::string& folder) {
  const SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder);
  if (hd.empty()) {
    ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
  }
  book(pHisto, hd);
  return;
}

void TCCPlotsBase::book(TH3*& pHisto, const SingleHistogramDefinition& hd) {
  if (hd.isValid()) {
    pHisto = Book3D(hd.name, hd.allTitles, 
		    hd.nBinsX, hd.xAxis.first, hd.xAxis.second, 
		    hd.nBinsY, hd.yAxis.first, hd.yAxis.second, 
		    hd.nBinsZ, hd.zAxis.first, hd.zAxis.second,
		    false);
  }
  return;
}

// 
void TCCPlotsBase::fillHisto(TH1* pTh1, const float value) {
  if (pTh1 and validArguments(value)) {
    pTh1->Fill(value);
  }
}

void TCCPlotsBase::fillHisto(TH1* pTh1, const float value, const float weight) {
  if (pTh1 and validArguments(value)) {
    pTh1->Fill(value, weight);
  }
}

//
void TCCPlotsBase::fillHisto(TH2* pTh2, const float xval, const float yval) {
  if (pTh2 and validArguments(xval, yval)) {
    pTh2->Fill(xval, yval);
  }
}

void TCCPlotsBase::fillHisto(TH2* pTh2, const float xval, const float yval, const float weight) {
  if (pTh2 and validArguments(xval, yval)) {
    pTh2->Fill(xval, yval, weight);
  }
}

//
void TCCPlotsBase::fillHisto(TH3* pTh3, const float xval, const float yval, const float zval) {
  if (pTh3 and validArguments(xval, yval, zval)) {
    pTh3->Fill(xval, yval, zval);
  }
}

void TCCPlotsBase::fillHisto(TH3* pTh3, const float xval, const float yval, const float zval, const float weight) {
  if (pTh3 and validArguments(xval, yval, zval)) {
    pTh3->Fill(xval, yval, zval, weight);
  }
}
  
SingleHistogramDefinition TCCPlotsBase::retrieveDefinition(const std::string& histoIdentifier, const std::string& folder) {
  SingleHistogramDefinition s; // invalid result
  
  ATH_MSG_DEBUG("Retrieving SingleHistogramDefinition for "<< histoIdentifier);

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
  return s;
}
