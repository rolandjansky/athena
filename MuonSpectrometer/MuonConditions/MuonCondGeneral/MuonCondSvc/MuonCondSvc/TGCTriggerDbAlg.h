/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERDBALG_H
#define TGCTRIGGERDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCondSvc/TGCTriggerData.h"

class TGCTriggerDbAlg: public AthAlgorithm
{
  public:
    TGCTriggerDbAlg (const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TGCTriggerDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;
 
  private:
    void fillReadMapBw(TGCTriggerData* writeCdo, const CondAttrListCollection* readKey);
    void fillTrigBitEifi(TGCTriggerData* writeCdo, const CondAttrListCollection* readKey);
    void fillTrigBitTile(TGCTriggerData* writeCdo, const CondAttrListCollection* readKey);

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_bw{this, "ReadKeyBw", "/TGC/TRIGGER/CW_BW", "SG key for CW-BW"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_eifi{this, "ReadKeyEifi", "/TGC/TRIGGER/CW_EIFI", "SG key for CW-EIFI"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_tile{this, "ReadKeyTile", "/TGC/TRIGGER/CW_TILE", "SG key for CW-TILE"};
    SG::WriteCondHandleKey<TGCTriggerData> m_writeKey{this, "WriteKey", "TGCTriggerData", "SG Key of TGCTrigger LUTs"};

    ServiceHandle<ICondSvc> m_condSvc;
};

#endif


