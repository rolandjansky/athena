/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOWEIGHTSFILL_H
#define LARGEOWEIGHTSFILL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"

class StoreGateSvc;
class LArOnlineID;

class LArGeoWeightsFill:public AthAlgorithm {
 public:
  LArGeoWeightsFill(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArGeoWeightsFill();
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};

  const LArOnlineID* m_onlineID;

  std::string m_nameOfSet;
  std::string m_key;

  bool m_fill, m_dump;
  std::string m_outFileName;

  ToolHandle < CaloTriggerTowerService > m_ttService;

};

#endif
