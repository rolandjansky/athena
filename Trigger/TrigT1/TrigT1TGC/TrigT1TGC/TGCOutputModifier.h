/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1TGC_TGCOUTPUTMODIFIER_H
#define TRIGT1TGC_TGCOUTPUTMODIFIER_H

/*
class description
*/

// Athena/Gaudi include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "TVector3.h"

namespace LVL1TGCTrigger {
  class TGCOutputModifier : public AthReentrantAlgorithm
  {
    
  public:
    
    TGCOutputModifier( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TGCOutputModifier();
    
    /// Regular Gaudi algorithm initialization function
    virtual StatusCode initialize() override;
    /// Regular Gaudi algorithm execute function
    virtual StatusCode execute(const EventContext& eventContext) const override;
    
  private:
    
    void Copy(const LVL1MUONIF::Lvl1MuSectorLogicDataPhase1* sldataOrg,
	      LVL1MUONIF::Lvl1MuSectorLogicDataPhase1& sldataNew  ) const;
    void Update(const std::vector<const xAOD::MuonSegment*>& segs,
		LVL1MUONIF::Lvl1MuSectorLogicDataPhase1& sldata,
		size_t systemAddress,
		size_t subSystemAddress,
		size_t sectorAddress) const;
    
    SG::ReadHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_inputKey{this,"InputMuctpiLocation","L1MuctpiStoreTGC","Input Tgc2Muctpi interface container"};
    SG::WriteHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_outputKey{this, "OutputMuctpiLocation", "L1MuctpiStoreTGCmod", "Output Tgc2Muctpi interface container"};
    ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recTGCRoiTool{this,"TrigT1TGCRecRoiTool","LVL1::TrigT1TGCRecRoiTool","hoge"};
    SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_musegKey{this,"MuonSegmentContainer","xaodMuonSegments","MuonSegmentContainer"};
    DoubleProperty m_deltaThetaCut1{this,"DeltaThetaCut1", 0.008,"Delta theta cut for MuonSegment in low-eta region"};
    DoubleProperty m_deltaEtaCut1{this,"DeltaEtaCut1", 0.09,"Delta eta cut for MuonSegment and RoI position in low-eta region"};
    DoubleProperty m_deltaPhiCut1{this,"DeltaPhiCut1", 0.16,"Delta phi cut for MuonSegment and RoI position in low-eta region"};
    DoubleProperty m_deltaThetaCut2{this,"DeltaThetaCut2", 0.001,"Delta theta cut for MuonSegment in high-eta region"};
    DoubleProperty m_deltaEtaCut2{this,"DeltaEtaCut2", 0.16,"Delta eta cut for MuonSegment and RoI position in high-eta region"};
    DoubleProperty m_deltaPhiCut2{this,"DeltaPhiCut2", 0.19,"Delta phi cut for MuonSegment and RoI position in high-eta region"};
    DoubleProperty m_etaBoundary1{this,"EtaBoundary1", 1.9,"Eta boundary where the cuts1 are applied"};
    DoubleProperty m_etaBoundary2{this,"EtaBoundary2", 2.1,"Eta boundary where the cuts2 are applied"};
    BooleanProperty m_emulateA{this,"EmulateA", true,"Emulate A-side"};
    BooleanProperty m_emulateC{this,"EmulateC", true,"Emulate C-side"};
    BooleanProperty m_nswVetoMode{this,"NSWVetoMode", true,"NSW veto mode or monitoring mode"};

  };
}

#endif
