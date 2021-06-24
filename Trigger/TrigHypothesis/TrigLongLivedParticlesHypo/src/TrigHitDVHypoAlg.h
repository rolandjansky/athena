/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H

#include "Gaudi/Property.h"

#include <string>
#include "TrigHitDVHypoTool.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODJet/JetContainer.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

/**
 * @class TrigHitDVHypoAlg
 * @brief Implements Hypo selection on triggering displaced vertex
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigHitDVHypoAlg : public ::HypoBase 
{ 
public: 

   TrigHitDVHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
   
   virtual StatusCode  initialize() override;
   virtual StatusCode  execute(const EventContext& context) const override;
   
private: 
   ToolHandleArray< TrigHitDVHypoTool >   m_hypoTools     {this, "HypoTools", {}, "Tools to perfrom selection"};

   // EDMs
   SG::ReadHandleKey< xAOD::JetContainer >          m_jetsKey     {this, "Jets",     "HLT_AntiKt4EMTopoJets_subjesIS", ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVTrkKey {this, "HitDVTrk", "HLT_HitDVTrk",  ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVSPKey  {this, "HitDVSP",  "HLT_HitDVSP",   ""};
   SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_hitDVKey    {this, "HitDV",    "HLT_HitDV",     ""};

   // Luminosity related
   ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;
   // Property: Per-bunch luminosity data (data only) conditions input)
   SG::ReadCondHandleKey<LuminosityCondData>        m_lumiDataKey {this, "LuminosityCondDataKey", "LuminosityCondData", ""};
   // Property; MC flag.
   Gaudi::Property<bool> m_isMC {this, "isMC", false, "Real data or MC"};
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H
