/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGISOHPTTRACKTRIGGERHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGISOHPTTRACKTRIGGERHYPOTOOL_H 1

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <string>

#include "TrigCompositeUtils/TrigCompositeUtils.h"
using namespace TrigCompositeUtils;

/**
 * @class TrigIsoHPtTrackTriggerHypoTool
 * @brief Generated the decision for hypo thats triggers on high pt isolated tracks 
 * @author Ismet Siral <ismet.siral@cern.ch> - University of Oregon
 **/

class TrigIsoHPtTrackTriggerHypoTool : virtual public ::AthAlgTool
{ 
 public: 
  TrigIsoHPtTrackTriggerHypoTool( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigIsoHPtTrackTriggerHypoTool();
  virtual StatusCode initialize() override;



  struct TrackInfo {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrackParticle_v1* track;
    const xAOD::TrackParticleContainer* AllTracks;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
  };

  /**
   * @brief decides upon a collection of tracks
   **/
  StatusCode decide( std::vector<TrackInfo>& decisions )  const;

  /**
   * @brief Auxiluary method, single track selection
   **/
  bool decideOnSingleObject( const xAOD::TrackParticle_v1 *track, const xAOD::TrackParticleContainer* AllTracks, size_t cutIndex ) const;

  /**
   * @brief actual implementation of decide, in case of inclusive selection ( one object cut )
   **/
  StatusCode inclusiveSelection( std::vector<TrackInfo>& input ) const;

  /**
   * @brief actual implementation of decide, in case of multiple objects selection ( independentone )
   **/
  StatusCode multiplicitySelection( std::vector<TrackInfo>& input ) const;

  /**
   * @brief stores decisions for all object passing multiple cuts
   * The passsingSelection inner vectors have to have size == input size
   **/
  StatusCode markPassing( std::vector<TrackInfo>& input, const std::set<size_t>& passing ) const;




  
 private:
  HLT::Identifier m_decisionId;
  /* Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" }; */
  Gaudi::Property< std::vector<float> > m_TrackPt{ this, "MinTrackPt",  { float( 50.0*Gaudi::Units::GeV ) }, "Track pT requirement" };
  Gaudi::Property< std::vector<float> > m_TrackEta{ this, "MinTrackEta",  { float(2.5 ) }, "Track Eta requirement" };
  Gaudi::Property< std::vector<float> > m_Trackd0{ this,  "MaxTrackd0", {5.}, "Maximum Track d0 allowed"      };
  Gaudi::Property< std::vector<float> > m_Trackd0Sig{ this,  "MaxTrackd0Sig", {5.}, "Maximum Track d0 Sig allowed"      };
  Gaudi::Property< std::vector<unsigned> > m_TrackNPixHits{ this,  "MinTrackNPixHits", {2}, "Minimum number of pixel hits required from the trigger"     }; 
  Gaudi::Property< std::vector<unsigned> > m_TrackNSCTHits{ this,  "MinTrackNSCTHits", {5}, "Minimum number of SCT hits required from the trigger"     }; 

  Gaudi::Property< std::vector<bool> > m_doIso { this,  "EnableTrackIsolation", {false}, "If track based isolation should be applied or not? "};
  Gaudi::Property< std::vector<bool> > m_IsoCum { this,  "EnableCumalitiveIsolation", {false}, "Instead of checking if one track is above a certain pT threshold, add up all tracks for isolation"};
  Gaudi::Property< std::vector<float> > m_IsoDR{ this,  "TrackIsoCone", {0.3}, "Isolation requirment over the main track" };
  Gaudi::Property< std::vector<float> > m_IsoPt{ this,  "MinIsoTrackPt", {float( 5.0*Gaudi::Units::GeV )}, "Min pT requirment of other tracks to be considered for isolation" };

  
  size_t m_multiplicity = 1;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGISOHPTTRACKTRIGGERHYPOTOOL_H
