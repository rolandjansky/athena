/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOWEIGHTSFILL_H
#define LARGEOWEIGHTSFILL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
class StoreGateSvc;
class LArOnlineID;

class LArGeoWeightsFill:public AthAlgorithm {
 public:
  LArGeoWeightsFill(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArGeoWeightsFill();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override {return StatusCode::SUCCESS;}
  virtual StatusCode stop() override;

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager","CaloDetDescrManager","SG Key for CaloDetDescrManager in the Condition Store" };

  const LArOnlineID* m_onlineID;

  std::string m_nameOfSet;
  std::string m_key;

  bool m_fill, m_dump;
  std::string m_outFileName;

  ToolHandle < CaloTriggerTowerService > m_ttService;

};

#endif
