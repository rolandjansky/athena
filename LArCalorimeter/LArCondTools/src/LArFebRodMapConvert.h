/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LARFEBRODMAPCONVERT_H
#define LARCONDITIONSTEST_LARFEBRODMAPCONVERT_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRecConditions/LArFebRodMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"

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
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;

 private: 

  SG::ReadCondHandleKey<LArFebRodMapping> m_cablingKey{this,"RodCablingKey","ArFebRodMap","SG Key of ROD mapping object"};

  /// Default constructor: 
  LArFebRodMapConvert();

  const LArOnlineID* m_onlineID;


}; 

#endif //> !LARCONDITIONSTEST_LARIDMAPCONVERT_H
