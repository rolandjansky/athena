/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JGTOWERMAKER_H
#define TRIGT1CALOFEXPERF_JGTOWERMAKER_H
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloTriggerTool/JTowerSCMap.h"
#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/JTower_ID.h"
#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "Identifier/IdentifierHash.h"
#include "JGTowerHelper.h"

namespace LVL1
{
  class JGTowerMaker : public ::AthAlgorithm
  {
  public:
    JGTowerMaker(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JGTowerMaker();

  private:
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode FexAlg(const std::vector<std::shared_ptr<JGTowerHelper>> &jgT, xAOD::JGTowerContainer *towerContainer);
    virtual StatusCode SCTowerMapping();
    virtual StatusCode ForwardMapping();
    virtual StatusCode TileMapping();

    std::vector<std::shared_ptr<JGTowerHelper>> m_jT;
    std::vector<std::shared_ptr<JGTowerHelper>> m_gT;

    GTowerSCMap *m_GMap = new GTowerSCMap();
    JTowerSCMap *m_JMap = new JTowerSCMap();

    const DataHandle<CaloSuperCellDetDescrManager> m_sem_mgr;

    //  const DataHandle<CaloTTDescrManager> m_tt_mgr; //kept temporarily to use trigger tower id for for ID mapping in the futher
    const CaloCell_SuperCell_ID *m_scid;
    const JTower_ID *m_jTowerId;
    const GTower_ID *m_gTowerId;
    //  const TTOnlineID* m_TTID; // same reason as above
    const Tile_SuperCell_ID *m_tid;
    const CaloCell_ID *m_ccIdHelper;
    bool m_useSCQuality;
    bool m_useAllCalo;
    bool m_useAllRun2TT;
    bool m_EmulateSC;
    bool m_TileMapped;
    float m_minSCETp;
    float m_maxSCETm;
    float m_minTowerEt;
    std::string m_scType;
    uint16_t m_scQuality;
  };
} // namespace LVL1

#endif //> !XAODL1CALO_XAODL1CALOALG_H
