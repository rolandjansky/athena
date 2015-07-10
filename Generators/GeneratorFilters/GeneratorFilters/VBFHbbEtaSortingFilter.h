/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/VBFHbbEtaSortingFilter.h
// Description:
//   The filter for VBFHbb four jet final state, by requiring four jets (2b + 2j and eta-sorting j < b < b < j)
//
// AuthorList:
//   Yasuyuki Okumura June 2015


#ifndef GENERATORFILTERSVBFHBBETASORTINGFILTER_H
#define GENERATORFILTERSVBFHBBETASORTINGFILTER_H

#include "GeneratorModules/GenFilter.h"
#include <string>

class VBFHbbEtaSortingFilter:public GenFilter {

 public:
  VBFHbbEtaSortingFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~VBFHbbEtaSortingFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

 private:
  // Setable Properties:
	
  // Local Member Data:
  bool   m_debug;
  double m_MinJetPt;  // minimum pT for jet selection in MeV
  double m_MaxJetEta;  // maximum eta for Jets (4.9 by default)
  std::string m_TruthJetContainerName;  // Name of the truth jet container
  std::string m_JetTruthLabelName;  // Name of truth label "ConeTruthLabelID" or "PartonTruthLabelID" default "PartonTruthLabelID"
	
  long m_total;    // Total number of events tested
  long m_passed_multiplicity;   // Number of events passing cuts
  long m_passed_all;   // Number of events passing cuts
  
  // Private Methods:

};

#endif
