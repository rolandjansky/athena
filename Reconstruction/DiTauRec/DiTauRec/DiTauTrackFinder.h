/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_TRACKFINDER_H
#define DITAUREC_TRACKFINDER_H

#include "DiTauToolBase.h"

#include "GaudiKernel/ToolHandle.h"

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

 virtual StatusCode initialize();

 virtual StatusCode execute(DiTauCandidateData * data);

 virtual StatusCode eventFinalize(DiTauCandidateData *data);


 virtual void cleanup(DiTauCandidateData *) { }
 
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
                       std::vector<const xAOD::TrackParticle*>& );

 DiTauTrackType diTauTrackType( const DiTauCandidateData*,
                                const xAOD::TrackParticle*,
                                const xAOD::Vertex* );




private:
 float m_MaxDrJet;
 float m_MaxDrSubjet;
 std::string m_TrackParticleContainerName;
 ToolHandle<Trk::ITrackSelectorTool> m_TrackSelectorTool;
 // ToolHandle< Trk::IParticleCaloExtensionTool > m_ParticleCaloExtensionTool;


};

#endif  /* TRACKFINDER_H */
