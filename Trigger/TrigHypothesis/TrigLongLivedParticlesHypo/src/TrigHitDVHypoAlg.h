/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H

#include <string>

#include "Gaudi/Property.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"
#include "DecisionHandling/HypoBase.h"
#include "TrigHitDVHypoTool.h"

#include "xAODJet/JetContainer.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "TMVA/Reader.h"

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
   SG::ReadHandleKey< xAOD::JetContainer >          m_jetsKey      {this, "Jets",      "HLT_AntiKt4EMTopoJets_subjesIS", ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVSeedKey {this, "HitDVSeed", "HLT_HitDVSeed", ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVTrkKey  {this, "HitDVTrk",  "HLT_HitDVTrk",  ""};
   SG::ReadHandleKey< xAOD::TrigCompositeContainer> m_hitDVSPKey   {this, "HitDVSP",   "HLT_HitDVSP",   ""};
   SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_hitDVKey     {this, "HitDV",     "HLT_HitDV",     ""};

   // Luminosity related
   ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;
   // Property: Per-bunch luminosity data (data only) conditions input)
   SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey {this, "LuminosityCondDataKey", "LuminosityCondData", ""};
   // Property: MC flag.
   Gaudi::Property<bool> m_isMC {this, "isMC", false, "Real data or MC"};
   // Property: jet seed cut
   Gaudi::Property<float> m_jetSeed_ptMin  {this, "jetSeed_ptMin",  50.0, "Minimum pT  for jet seed"};
   Gaudi::Property<float> m_jetSeed_etaMax {this, "jetSeed_etaMin",  2.0, "Maximum eta for jet seed"};
   // Property: Name of the link to hitDV seed container to pass to ViewCreatorROITool
   Gaudi::Property<std::string>  m_hitDVLinkName {this, "hitDVLinkName", "HitDVSeedLink", "Name of the link to HitDVContainer. Used by ViewCreatorROITool."};

   // seed type enum
   using SeedType = TrigHitDVHypoTool::SeedType;

   // monitoring
   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
   StatusCode doMonitor(const xAOD::TrigCompositeContainer*) const;

   //
   float      deltaR(float, float, float, float) const;
   int        getSPLayer(int, float) const;
   StatusCode findSPSeeds(const EventContext&, const xAOD::TrigCompositeContainer*, std::vector<float>&, std::vector<float>&) const;
   StatusCode findJetSeeds(const xAOD::JetContainer*, const float, const float, std::vector<float>&, std::vector<float>&, std::vector<float>&) const;
   StatusCode selectSeedsNearby(const xAOD::TrigCompositeContainer* hitDVSeedsContainer,
				std::vector<float>& jetSeeds_pt, std::vector<float>& jetSeeds_eta, std::vector<float>& jetSeeds_phi) const;
   StatusCode calculateBDT(const EventContext&, const xAOD::TrigCompositeContainer*, const xAOD::TrigCompositeContainer*,
			   const std::vector<float>&, const std::vector<float>&, const std::vector<float>&,
			   const float&, const int, xAOD::TrigCompositeContainer*, int&) const;

   // BDT
   struct TMVAReader {
      std::unique_ptr<TMVA::Reader> tmva_0eta1;
      std::unique_ptr<TMVA::Reader> tmva_1eta2;
      float n_track_qual;
      float ly0_sp_frac;
      float ly1_sp_frac;
      float ly2_sp_frac;
      float ly3_sp_frac;
      float ly4_sp_frac;
      float ly5_sp_frac;
      float ly6_sp_frac;
      float ly7_sp_frac;
   };
   mutable SG::SlotSpecificObj<TMVAReader> m_tmva_reader ATLAS_THREAD_SAFE;

};

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOALG_H
