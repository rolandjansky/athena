/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOTOOL_H

#include <string>

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"

#include "TMVA/Reader.h"

/**
 * @class TrigHitDVHypoTool
 * @brief Generated the decision for hypo thats triggers on dispalced vertex
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigHitDVHypoTool : virtual public ::AthAlgTool
{ 
public: 
   TrigHitDVHypoTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent );
   
   virtual ~TrigHitDVHypoTool();
   virtual StatusCode initialize() override;
   
   struct TrackInfo {
      TrigCompositeUtils::Decision*       decision;
      float                               averageMu;
      const xAOD::JetContainer*           jetsContainer;
      const xAOD::TrigCompositeContainer* trksContainer;
      const xAOD::TrigCompositeContainer* spsContainer;
      xAOD::TrigCompositeContainer*       dvContainer;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
   };
   
   /**
    * @brief decides upon a collection of tracks
    **/
   StatusCode decide( TrackInfo& input )  const;

private:
   HLT::Identifier m_decisionId;
   Gaudi::Property< std::vector<float> >  m_cutJetPtGeV { this, "cutJetPtGeV", { float(20.0) }, "Jet pT requirement in GeV" };
   Gaudi::Property< std::vector<float> >  m_cutJetEta   { this, "cutJetEta",   { float(2.5) },  "Jet Eta requirement" };
   Gaudi::Property< std::vector<bool>  >  m_doSPseed    { this, "doSPseed",    { true },  "Switch to do SP seeding" };

   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
   
   //
   float      deltaR(float, float, float, float) const;
   int        getSPLayer(int, float) const;
   StatusCode findSPSeeds( const xAOD::TrigCompositeContainer*, std::vector<float>&, std::vector<float>& ) const;
   StatusCode findJetSeeds(const xAOD::JetContainer*, const float, const float, std::vector<float>&, std::vector<float>&) const;
   StatusCode calculateBDT(const xAOD::TrigCompositeContainer*, const xAOD::TrigCompositeContainer*,
			   const std::vector<float>&, const std::vector<float>&, const float, const int, xAOD::TrigCompositeContainer*, int&) const;
   float      getBDTthreshold(float) const;

   //
   TMVA::Reader* m_tmva_reader;
   mutable float m_tmva_n_track_qual ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly0_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly1_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly2_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly3_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly4_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly5_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly6_sp_frac  ATLAS_THREAD_SAFE;
   mutable float m_tmva_ly7_sp_frac  ATLAS_THREAD_SAFE;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGHITDVHYPOTOOL_H
