// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleCnvAlg.h 297747 2013-10-28 15:14:24Z krasznaa $
#ifndef XAODCREATORALGS_TRACKPARTICLECREATOR_H
#define XAODCREATORALGS_TRACKPARTICLECREATOR_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkValInterfaces/ITrkObserverTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "StoreGate/WriteDecorHandleKey.h"
// Local include(s):
#include "xAODTrackingCnv/ITrackParticleMonitoring.h"
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"
#include "xAODTrackingCnv/ITrackCollectionCnvTool.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"



namespace xAODMaker {

  /**
   *  @short Algorithm creating xAOD::TrackParticles from TrackParticles
   *
   *         This algorithm can be used to translate the TrackParticles coming
   *         from an AOD, and create xAOD::TrackParticle objects out of them
   *         for an output xAOD.
   *
   * @author Edward Moyse <Edward.Moyse@cern.ch>
   * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *
   * $Revision: 297747 $
   * $Date: 2013-10-28 16:14:24 +0100 (Mon, 28 Oct 2013) $
   */
  class TrackParticleCnvAlg : public AthReentrantAlgorithm {

  public:
    /// Regular algorithm constructor
    TrackParticleCnvAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute(const EventContext& ctx) const;

    virtual StatusCode finalize();

  private:
  
    /// toggle on adding truth links
    Gaudi::Property<bool> m_addTruthLink{this,"AddTruthLink", false };
    /// The key for the input TrackParticleTruthCollection


    /// ToolHandle to particle creator
    ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator{this,  "TrackParticleCreator", "Trk::TrackParticleCreatorTool/TrackParticleCreatorTool" };
    /// ToolHandle to truth classifier
    ToolHandle<IMCTruthClassifier> m_truthClassifier{
      this,
      "MCTruthClassifier",
      "MCTruthClassifier/MCTruthClassifier",
      " MCTruthClassifier Instance to use "
    };

    // handles to the converting tools
    ToolHandle<xAODMaker::ITrackCollectionCnvTool> m_TrackCollectionCnvTool{this, "TrackCollectionCnvTool", "xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool"};
    ToolHandle<xAODMaker::IRecTrackParticleContainerCnvTool>
      m_RecTrackParticleContainerCnvTool{this, "RecTrackParticleContainerCnvTool", "xAODMaker::RecTrackParticleContainerCnvTool/"
      "RecTrackParticleContainerCnvTool" };

    SG::ReadHandleKey<Rec::TrackParticleContainer> m_aod{this, "AODContainerName", "TrackParticleCandidate"};

    SG::ReadHandleKey<TrackCollection> m_tracks{this, "TrackContainerName", "Tracks"};

    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_xaodout{this, "xAODTrackParticlesFromTracksContainerName", "InDetTrackParticles"};
    

    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_xaodTrackParticlesout{this, "xAODContainerName", "ConvertedTrackParticleCandidate" };
    SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_xaodTruthOriginKey{this,"truthOriginKey", "" , "Key to declare that the alg will write truthOrigin. Will be overwritten during init"};
    SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_xaodTruthTypeKey{this, "truthTypeKey", "" , "Key to declare that the alg will write truthType. Will be overwritten during init" };
    SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_xaodTruthLinkKey{this, "truthLinkKey", "", "Key to declare that the alg will writhe the truthParticleLink. Will be overwritten during init."};
    SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> m_xaodTruthMatchProbKey{this, "truthMatchProbKey", "", "Key to declare that the alg will writhe the truthParticleLink. Will be overwritten during init."};


    SG::ReadHandleKey<xAODTruthParticleLinkVector> m_truthParticleLinkVec{this, "xAODTruthLinkVector", "xAODTruthLinks"};
    SG::ReadHandleKey<TrackParticleTruthCollection> m_aodTruth{this, "AODTruthContainerName" , ""};
    SG::ReadHandleKey<TrackTruthCollection> m_trackTruth{this, "TrackTruthContainerName", ""};
  
    // Allow monitoring of track parameters during conversion
    Gaudi::Property<bool> m_doMonitoring{this, "DoMonitoring", false};
    ToolHandle<ITrackParticleMonitoring> m_trackMonitoringTool{ this, "TrkMonTool", "", "Tracking Monitoring tool" };

    //for timing we need a handle to the MonTool in the alg 
    ToolHandle<GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

    // Augment observed tracks with information from track observer tool map
    Gaudi::Property<bool> m_augmentObservedTracks{this, "AugmentObservedTracks", false, "augment observed tracks"};
    SG::ReadHandleKey<ObservedTrackMap> m_tracksMap{this, "TracksMapName", "" , "name of observed tracks map saved in store"};


    /// toggle on converting AOD track particles to xAOD
    Gaudi::Property<bool> m_convertAODTrackParticles{this, "ConvertTrackParticles", true};

    /// toggle on converting tracks to xAOD
    Gaudi::Property<bool> m_convertTracks{this, "ConvertTracks", false};

    template<typename CONT, typename TRUTHCONT, typename CONVTOOL>
    int convert(const CONT&,
                const TRUTHCONT&,
                CONVTOOL& tool,
                SG::WriteHandle<xAOD::TrackParticleContainer>&,
                const xAODTruthParticleLinkVector*,
                const ObservedTrackMap* obs_track_map = 0) const;

    inline xAOD::TrackParticle* createParticle(
      xAOD::TrackParticleContainer& xaod,
      const Rec::TrackParticleContainer& container,
      const Rec::TrackParticle& tp);
    inline xAOD::TrackParticle* createParticle(
      xAOD::TrackParticleContainer& xaod,
      const TrackCollection& container,
      const Trk::Track& tp);

    }; // class TrackParticleCnvAlg

} // namespace xAODMaker

#endif // XAODCREATORALGS_TRACKPARTICLECREATOR_H
