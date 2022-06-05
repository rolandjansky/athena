/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LARIDMAPCONVERT_H
#define LARCONDITIONSTEST_LARIDMAPCONVERT_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"

#include <iostream>

class LArIdMapConvert
  : public ::AthAlgorithm
{ 

 public: 

  /// Constructor with parameters: 
  LArIdMapConvert( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArIdMapConvert(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;

 private: 

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "SCCalibLineKey", "LArCalibLineMapSC", "SG calib line key"};

  /// Default constructor: 
  LArIdMapConvert();
  void print (const HWIdentifier& hwid, std::ostream& out=std::cout); 

  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_caloCellID;


}; 

#endif //> !LARCONDITIONSTEST_LARIDMAPCONVERT_H
