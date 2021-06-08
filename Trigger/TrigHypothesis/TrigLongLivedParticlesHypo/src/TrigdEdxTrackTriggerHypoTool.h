/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKTRIGGERHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKTRIGGERHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <string>

/**
 * @class TrigdEdxTrackTriggerHypoTool
 * @brief Generated the decision for hypo thats triggers on high pt isolated tracks 
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class TrigdEdxTrackTriggerHypoTool : virtual public ::AthAlgTool
{ 
public: 
   TrigdEdxTrackTriggerHypoTool( const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent );
   
   virtual ~TrigdEdxTrackTriggerHypoTool();
   virtual StatusCode initialize() override;
   
   struct TrackInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigCompositeContainer* trksContainer;
      const xAOD::TrigCompositeContainer* hitsContainer;
      xAOD::TrigCompositeContainer& dedxContainer;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
   };
   
   /**
    * @brief decides upon a collection of tracks
    **/
   StatusCode decide( TrackInfo& input )  const;

private:
   HLT::Identifier m_decisionId;
   Gaudi::Property< std::vector<float> >  m_cutTrackPtGeV        { this, "cutTrackPtGeV",         { float(20.0) }, "Track pT requirement in GeV" };
   Gaudi::Property< std::vector<float> >  m_cutTrackEta          { this, "cutTrackEta",           { float(2.5) },  "Track Eta requirement" };
   Gaudi::Property< std::vector<float> >  m_cutTracka0beam       { this, "cutTracka0beam",        { float(2.5) },  "Track a0beam requirement" };
   Gaudi::Property< std::vector<float> >  m_cutTrackdEdx         { this, "cutTrackdEdx",          { float(1.7) },  "Track dE/dx requirement" };
   Gaudi::Property< std::vector<int> >    m_cutTrackNhighdEdxHits{ this, "cutTrackNhighdEdxHits", { int(2) },      "N high dEdx of pixel hits requirement" }; 
   Gaudi::Property< std::vector<float> >  m_cutTrackHighdEdxDef  { this, "cutTrackHighdEdxDef",   { float(1.7) },  "Definition / threshold of high dEdx hits" };
  
   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDEDXTRACKTRIGGERHYPOTOOL_H
