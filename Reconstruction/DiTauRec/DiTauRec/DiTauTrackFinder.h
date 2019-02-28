/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_TRACKFINDER_H
#define DITAUREC_TRACKFINDER_H

#include "DiTauToolBase.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"


class DiTauTrackFinder : public DiTauToolBase {
public:

 //-------------------------------------------------------------
 //! Constructor
 //-------------------------------------------------------------
 DiTauTrackFinder(const std::string& type,
         const std::string& name,
         const IInterface * parent);

 //-------------------------------------------------------------
 //! Destructor
 //-------------------------------------------------------------
 virtual ~DiTauTrackFinder();

 virtual StatusCode initialize() override;

 virtual StatusCode execute(DiTauCandidateData * data,
                            const EventContext& ctx) const override;

 virtual void cleanup(DiTauCandidateData *) override { }
 
 // ------------------------------------------------------------
 // Definition of track types
 // ------------------------------------------------------------
 enum DiTauTrackType { 
 	DiTauSubjetTrack = 0,
	DiTauIsoTrack = 1,
	DiTauOtherTrack = 2,
	OutsideTrack = 3
 };

 void getTracksFromPV( const DiTauCandidateData*,
                       const xAOD::TrackParticleContainer*,
                       const xAOD::Vertex*,
                       std::vector<const xAOD::TrackParticle*>&,
                       std::vector<const xAOD::TrackParticle*>&,
                       std::vector<const xAOD::TrackParticle*>& ) const;

 DiTauTrackType diTauTrackType( const DiTauCandidateData*,
                                const xAOD::TrackParticle*,
                                const xAOD::Vertex* ) const;




private:
 float m_MaxDrJet;
 float m_MaxDrSubjet;
 SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticleContainerName
 { this, "TrackParticleContainer", "InDetTrackParticles", "" };
 ToolHandle<Trk::ITrackSelectorTool> m_TrackSelectorTool;
 // ToolHandle< Trk::IParticleCaloExtensionTool > m_ParticleCaloExtensionTool;


};

#endif  /* TRACKFINDER_H */
