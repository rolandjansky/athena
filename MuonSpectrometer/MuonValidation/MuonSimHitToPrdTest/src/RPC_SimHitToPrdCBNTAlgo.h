/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMHITTOPRD_H
#define MUONSIMHITTOPRD_H

#include <string.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "StoreGate/ReadHandleKey.h"
class RpcHitIdHelper;

namespace MuonGM {
    class MuonDetectorManager;
}

class RPC_SimHitToPrdCBNTAlgo : public AthAlgorithm {
public:
    // Agorithm constructor
    RPC_SimHitToPrdCBNTAlgo(const std::string &name, ISvcLocator *pSvcLocator);

    ~RPC_SimHitToPrdCBNTAlgo();

    // Gaudi hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

protected:
    void clearNTuple();

    SG::ReadHandleKey<McEventCollection> m_mcEvtKey{this, "mcEventKey", "TruthEvent"};
    SG::ReadHandleKey<RPCSimHitCollection> m_HitCollKey{this, "HitCollectionName", "RPC_Hits"};
    SG::ReadHandleKey<RpcDigitContainer> m_DigiCollKey{this, "DigitCollectionName", "RPC_DIGITS"};
    SG::ReadHandleKey<RpcPadContainer> m_RDOKey{this, "RDOCollectionName", "RPCPAD"};
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_PrepKey{this, "PrdCollectionName", "RPC_Measurements"};

private:
    // Private function to add the clusters to the ntuple
    StatusCode doMCtruth(const EventContext &ctx);
    StatusCode doRPCSimHit(const EventContext &ctx);
    StatusCode doRPCDigit(const EventContext &ctx);
    StatusCode doRPCRDO(const EventContext &ctx);
    StatusCode doRPCPrep(const EventContext &ctx);

    const MuonGM::MuonDetectorManager *m_muonMgr{nullptr};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    RpcHitIdHelper *m_muonHelper{nullptr};  // simulation id helper

    Gaudi::Property<bool> m_doMCtruth{this, "doMCtruth", true};
    Gaudi::Property<bool> m_doRPCSimHit{this, "doRPCSimHit", true};
    Gaudi::Property<bool> m_doRPCDigit{this, "doRPCDigit", true};
    Gaudi::Property<bool> m_doRPCRDO{this, "doRPCRDO", true};
    Gaudi::Property<bool> m_doRPCPrep{this, "doRPCPrep", true};

    class Clockwork;

    std::unique_ptr<Clockwork> m_c;
};
#endif  // MUONSIMHITTOPRD_H
