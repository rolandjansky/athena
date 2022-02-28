/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H

#include <string>

#include "Gaudi/Property.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigHitDVHypoTool.h"

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

   ToolHandleArray< TrigHitDVHypoTool >   m_hypoTools     {this, "HypoTools", {}, "Tools to perform selection"};

   // EDMs
   SG::ReadHandleKey< xAOD::JetContainer >          m_jetsKey     {this, "Jets",     "HLT_AntiKt4EMTopoJets_subjesIS", ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVTrkKey {this, "HitDVTrk", "HLT_HitDVTrk",  ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVSPKey  {this, "HitDVSP",  "HLT_HitDVSP",   ""};
   SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_hitDVKey    {this, "HitDV",    "HLT_HitDV",     ""};

   // Luminosity related
   ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;
   // Property: Per-bunch luminosity data (data only) conditions input)
   SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey {this, "LuminosityCondDataKey", "LuminosityCondData", ""};
   // Property; MC flag.
   Gaudi::Property<bool> m_isMC {this, "isMC", false, "Real data or MC"};

   // monitoring
   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
   StatusCode doMonitor(const xAOD::TrigCompositeContainer*) const;

   //
   float      deltaR(float, float, float, float) const;
   int        getSPLayer(int, float) const;
   StatusCode findSPSeeds(const EventContext&, const xAOD::TrigCompositeContainer*, std::vector<float>&, std::vector<float>&) const;
   StatusCode findJetSeeds(const xAOD::JetContainer*, const float, const float, std::vector<float>&, std::vector<float>&, std::vector<float>&) const;
   StatusCode calculateBDT(const EventContext&, const xAOD::TrigCompositeContainer*, const xAOD::TrigCompositeContainer*,
			   const std::vector<float>&, const std::vector<float>&, const std::vector<float>&,
			   const float&, const int, xAOD::TrigCompositeContainer*, int&) const;

   // BDT
   mutable SG::SlotSpecificObj<std::unique_ptr<TMVA::Reader> > m_tmva_reader ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_n_track_qual ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly0_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly1_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly2_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly3_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly4_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly5_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly6_sp_frac  ATLAS_THREAD_SAFE;
   mutable SG::SlotSpecificObj<float> m_tmva_ly7_sp_frac  ATLAS_THREAD_SAFE;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H
