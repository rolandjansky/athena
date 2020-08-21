// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Local include(s):
#include "xAODTrackingCnv/ITrackParticleMonitoring.h"




namespace Trk {
  class ITrackParticleCreatorTool;
}
class IMCTruthClassifier;
namespace xAODMaker {
  class ITrackCollectionCnvTool;
  class IRecTrackParticleContainerCnvTool;
}


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
    /// The key of the input TrackParticlesContainer

          
    /// toggle on adding truth links
    bool m_addTruthLink;
    /// The key for the input TrackParticleTruthCollection


    /// ToolHandle to particle creator
    ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator;
    /// ToolHandle to truth classifier
    ToolHandle<IMCTruthClassifier> m_truthClassifier;

    // handles to the converting tools
    ToolHandle< xAODMaker::ITrackCollectionCnvTool > m_TrackCollectionCnvTool;
    ToolHandle< xAODMaker::IRecTrackParticleContainerCnvTool > m_RecTrackParticleContainerCnvTool;

    SG::ReadHandleKey<Rec::TrackParticleContainer> m_aod;
    
    SG::ReadHandleKey<TrackCollection> m_tracks;

    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_xaodout;

    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_xaodTrackParticlesout;

    SG::ReadHandleKey<xAODTruthParticleLinkVector> m_truthParticleLinkVec;    
    SG::ReadHandleKey<TrackParticleTruthCollection> m_aodTruth;
    SG::ReadHandleKey<TrackTruthCollection>  m_trackTruth;    


    //Online Track monitoring
    bool m_doMonitoring;
    ToolHandle< ITrackParticleMonitoring > m_trackMonitoringTool;



    /// toggle on converting AOD track particles to xAOD
    bool m_convertAODTrackParticles;
      
    /// toggle on converting tracks to xAOD
    bool m_convertTracks;
      
    template <typename CONT, typename TRUTHCONT, typename CONVTOOL>
    int convert(const CONT&, const TRUTHCONT&, CONVTOOL& tool, SG::WriteHandle<xAOD::TrackParticleContainer>&, const xAODTruthParticleLinkVector*) const;
      
    inline xAOD::TrackParticle* createParticle(xAOD::TrackParticleContainer& xaod, const Rec::TrackParticleContainer& container, const Rec::TrackParticle& tp) ;
    inline xAOD::TrackParticle* createParticle( xAOD::TrackParticleContainer& xaod, const TrackCollection& container, const Trk::Track& tp) ;
    
         
        
  }; // class TrackParticleCnvAlg
  
} // namespace xAODMaker

#endif // XAODCREATORALGS_TRACKPARTICLECREATOR_H
