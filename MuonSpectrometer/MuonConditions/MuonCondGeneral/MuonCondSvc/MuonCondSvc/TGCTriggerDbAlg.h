/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERDBALG_H
#define TGCTRIGGERDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCondSvc/TGCTriggerData.h"

class TGCTriggerDbAlg: public AthAlgorithm {

  public:

    TGCTriggerDbAlg (const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TGCTriggerDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;
 
  protected:

    enum {TMap_HH=0, TMap_HL, TMap_LH, TMap_LL, N_TMap};
    int getTYPE(int lDR, int hDR, int lDPhi, int hDPhi) const;
    int SUBSECTORADD(int ssid, int modid, int phimod2, int type) const;
 
  private:
    
    void loadParameters(TGCTriggerData* writeCdo,
                        const CondAttrListCollection* readKey,
                        int cw_type);

    void fillReadMapBw(TGCTriggerData* writeCdo);
    void fillTrigBitEifi(TGCTriggerData* writeCdo);
    void fillTrigBitTile(TGCTriggerData* writeCdo);


    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_bw;
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_eifi;
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_tile;
    SG::WriteCondHandleKey<TGCTriggerData> m_writeKey;    
    ServiceHandle<ICondSvc> m_condSvc;

};

#endif


