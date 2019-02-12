///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LARFEBRODMAPCONVERT_H
#define LARCONDITIONSTEST_LARFEBRODMAPCONVERT_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArFebRodMapping.h"

#include <iostream>

class LArFebRodMapConvert
  : public ::AthAlgorithm
{ 

 public: 

  /// Constructor with parameters: 
  LArFebRodMapConvert( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArFebRodMapConvert(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

  /// Default constructor: 
  LArFebRodMapConvert();

  const LArOnlineID* m_onlineID;

  SG::ReadCondHandleKey<LArFebRodMapping>  m_RodKey{this, "FebRodKey", "LArFebRodMap", "SG ROD mapping key"};

}; 

#endif //> !LARCONDITIONSTEST_LARIDMAPCONVERT_H
