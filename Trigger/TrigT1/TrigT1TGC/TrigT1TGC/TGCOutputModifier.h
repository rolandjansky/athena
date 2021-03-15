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
    struct MuSegData{
      TVector3 pos;
      TVector3 vec;
    };
    void Update(const std::vector<MuSegData>&,
		LVL1MUONIF::Lvl1MuSectorLogicDataPhase1& sldata,
		size_t systemAddress,
		size_t subSystemAddress,
		size_t sectorAddress) const;
    
    SG::ReadHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_inputKey{this,"InputMuctpiLocation","L1MuctpiStoreTGC","Input Tgc2Muctpi interface container"};
    SG::WriteHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_outputKey{this, "OutputMuctpiLocation", "L1MuctpiStoreTGCmod", "Output Tgc2Muctpi interface container"};
    ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recTGCRoiTool{this,"TrigT1TGCRecRoiTool","LVL1::TrigT1TGCRecRoiTool","hoge"};
    SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_musegKey{this,"MuonSegmentContainer","xaodMuonSegments","MuonSegmentContainer"};
    DoubleProperty m_deltaEtaCut{this,"DeltaEtaCut", 0.25,"Delta eta cut for MuonSegment and RoI position"};
    DoubleProperty m_deltaPhiCut{this,"DeltaPhiCut", 0.20,"Delta phi cut for MuonSegment and RoI position"};
    DoubleProperty m_deltaThetaCut{this,"DeltaThetaCut", 999.,"Delta theta cut for MuonSegment"};
    BooleanProperty m_emulateA{this,"EmulateA", true,"Emulate A-side"};
    BooleanProperty m_emulateC{this,"EmulateC", true,"Emulate C-side"};

  };
}

#endif
