/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBDMCONTAINERSPLITTER_H
#define TBREC_TBDMCONTAINERSPLITTER_H

// 
// class TBDMContainerSplitter 
// Splitting the CalibDMContainer hits to two, leaving all leakage in second
// both are VIEW containers....
//
#include "AthenaBaseComps/AthAlgorithm.h"

class CaloDmDescrManager;

class TBDMContainerSplitter: public AthAlgorithm {
 public:    
  
  TBDMContainerSplitter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBDMContainerSplitter();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private: 
  // Names and pointers
  std::string m_DMHitsKey;               // Original DM hits container key
  std::string m_DMHitsCaloKey;               // DM hits in calo modules  container key
  std::string m_DMHitsLeakKey;               // DM hits for leakage container key

  const CaloDmDescrManager* m_caloDmDescrManager; 
};

#endif
