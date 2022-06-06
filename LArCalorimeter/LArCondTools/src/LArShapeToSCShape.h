/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LARSHAPETOSCSHAPE_H
#define LARCONDITIONSTEST_LARSHAPETOSCSHAPE_H 1

// STL includes
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "GaudiKernel/ToolHandle.h"

class LArShapeToSCShape
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  LArShapeToSCShape( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArShapeToSCShape(); 

  // Assignment operator: 
  //LArShapeToSCShape &operator=(const LArShapeToSCShape &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;

 private: 

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"SCCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  /// Default constructor: 
  LArShapeToSCShape();

  ToolHandle<ICaloSuperCellIDTool> m_scidTool{this, "CaloSuperCellIDTool", "CaloSuperCellIDTool"};

  

}; 

#endif //> !LARCONDITIONSTEST_LARSHAPETOSCSHAPE_H
