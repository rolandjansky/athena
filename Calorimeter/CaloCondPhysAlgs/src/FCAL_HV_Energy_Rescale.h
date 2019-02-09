///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDPHYSALGS_FCAL_HV_ENERGY_RESCALE_H
#define CALOCONDPHYSALGS_FCAL_HV_ENERGY_RESCALE_H 1
// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArHVCorrTool.h"
#include "LArCabling/LArOnOffIdMapping.h"

class FCAL_HV_Energy_Rescale: public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  FCAL_HV_Energy_Rescale( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~FCAL_HV_Energy_Rescale(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  virtual StatusCode  stop();

private:
  ToolHandle<ILArHVCorrTool> m_hvCorrTool;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  std::string m_folder;

}; 

#endif //> !CALOCONDPHYSALGS_FCAL_HV_ENERGY_RESCALE_H
