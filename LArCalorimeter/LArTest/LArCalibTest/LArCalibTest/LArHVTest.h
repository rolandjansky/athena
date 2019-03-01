/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVTEST_H
#define LARHVTEST_H

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArHVTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/DataHandle.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArID.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "LArRecConditions/LArHVData.h"

class StoreGateSvc; 
class LArElectrodeID;

class LArHVTest: public AthAlgorithm
{
 public:
  
  // constructor
  LArHVTest(const std::string & name, ISvcLocator * pSvcLocator); 
  
  // destructor 
  virtual ~LArHVTest(){};
  
  // initialize and finalize methods
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute() override;


 private:

  const LArOnlineID*    m_lar_on_id; 	
  const CaloCell_ID*    m_calocell_id;	
  const LArEM_ID*       m_larem_id;	
  const LArHEC_ID*      m_larhec_id;	
  const LArFCAL_ID*     m_larfcal_id;	
  const LArElectrodeID* m_electrodeID;  

  ToolHandle<ILArHVTool> m_hvtool;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "CablingKey", "LArOnOffIdMap","Mapping key"};
  SG::ReadCondHandleKey<LArHVIdMapping> m_hvmapKey{this, "HVMapKey", "LArHVIdMap", "Key for mapping object" };
  SG::ReadCondHandleKey<LArHVData> m_hvdataKey{this, "HVDataKey", "LArHVData", "Key to retrieve HV data"};


};

#endif
