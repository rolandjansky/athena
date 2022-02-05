// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRecUtils/src/LArRoIMapCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2021
 * @brief Conditions algorithm producing LArRoIMap.
 */


#ifndef LARRECUTILS_LARROIMAPCONDALG_H
#define LARRECUTILS_LARROIMAPCONDALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "LArRecConditions/LArRoIMap.h"
#include "LArRecConditions/LArFebRodMapping.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h" 
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


class LArOnlineID; 
class CaloLVL1_TT_ID;
class LArEM_ID; 
class LArHEC_ID; 
class LArFCAL_ID; 


/**
 * @brief Conditions algorithm producing LArRoIMap.
 */
class LArRoIMapCondAlg
  : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  
  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Gaudi execute method.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;

  
private:
  StatusCode addEntry (LArRoIMap& m,
                       const LArOnOffIdMapping& onOffId,
                       const LArFebRodMapping& febRod,
                       const Identifier& id) const;

  const CaloLVL1_ID* m_TT_ID = nullptr; 
  const LArOnlineID* m_onlineID = nullptr; 
  const LArEM_ID*    m_em_id = nullptr; 
  const LArHEC_ID*   m_hec_id = nullptr ;
  const LArFCAL_ID*  m_fcal_id = nullptr; 

  ToolHandle<CaloTriggerTowerService> m_ttSvc
    { this, "TriggerTowerSvc", "CaloTriggerTowerService", "Towers tool"};

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_onOffIdKey
    { this, "LArOnOffIdMap", "LArOnOffIdMap", "LArOnOffIdMapping object" };
  SG::ReadCondHandleKey<LArFebRodMapping> m_febRodKey
    { this, "LArFebRodMap", "LArFebRodMap", "LArFebRodMap object" };
  SG::WriteCondHandleKey<LArRoIMap> m_LArRoIMapKey
    { this, "LArRoIMapKey", "LArRoIMap", "LArRoIMap key" };
};


#endif // not LARRECUTILS_LARROIMAPCONDALG_H
