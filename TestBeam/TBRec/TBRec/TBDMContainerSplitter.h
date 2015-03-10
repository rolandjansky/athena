/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBDMCONTAINERSPLITTER
#define TBDMCONTAINERSPLITTER

// 
// class TBDMContainerSplitter 
// Splitting the CalibDMContainer hits to two, leaving all leakage in second
// both are VIEW containers....
//
#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
class CaloDmDescrManager;

class TBDMContainerSplitter: public AthAlgorithm {
 public:    
  
  TBDMContainerSplitter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBDMContainerSplitter();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private: 
  // Names and pointers
  std::string m_DMHitsKey;               // Original DM hits container key
  std::string m_DMHitsCaloKey;               // DM hits in calo modules  container key
  std::string m_DMHitsLeakKey;               // DM hits for leakage container key

  const CaloDmDescrManager* m_caloDmDescrManager; 
};

#endif
