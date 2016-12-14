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
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
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
  class TrackParticleCnvAlg : public AthAlgorithm {

  public:
    /// Regular algorithm constructor
    TrackParticleCnvAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute();

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

    SG::ReadHandle<Rec::TrackParticleContainer> m_aod;
    
    SG::ReadHandle<TrackCollection> m_tracks;

    SG::WriteHandle<xAOD::TrackParticleContainer> m_xaodout;
    SG::WriteHandle<xAOD::TrackParticleAuxContainer> m_xauxout;

    SG::WriteHandle<xAOD::TrackParticleContainer> m_xaodTrackParticlesout;
    SG::WriteHandle<xAOD::TrackParticleAuxContainer> m_xauxTrackParticlesout;

    SG::ReadHandle<xAODTruthParticleLinkVector> m_truthParticleLinkVec;    
    SG::ReadHandle<TrackParticleTruthCollection> m_aodTruth;
    SG::ReadHandle<TrackTruthCollection>  m_trackTruth;    

    /// toggle on converting AOD track particles to xAOD
    bool m_convertAODTrackParticles;
      
    /// toggle on converting tracks to xAOD
    bool m_convertTracks;
      
    template <typename CONT, typename TRUTHCONT, typename CONVTOOL>
    int convert(const CONT&, const TRUTHCONT&, CONVTOOL& tool, SG::WriteHandle<xAOD::TrackParticleContainer>&);
      
    inline xAOD::TrackParticle* createParticle(xAOD::TrackParticleContainer& xaod, const Rec::TrackParticleContainer& container, const Rec::TrackParticle& tp) ;
    inline xAOD::TrackParticle* createParticle( xAOD::TrackParticleContainer& xaod, const TrackCollection& container, const Trk::Track& tp) ;
    
         
    bool m_IdOutputInfo;
     
    unsigned int m_numEvents;
    /** the number of Trk::Tracks processed, this is equal to the sum of tracks over all events in the input TrackContainer */
    unsigned long m_nTracksProcessed;        
    /** the number of Rec::TrackParticle created, should be the same as Trk::Tracks processed but one never knows! */
    unsigned long m_nTrackParticlesCreated;
     
    unsigned int  m_numberOfBLayerHits;
    unsigned int  m_numberOfBLayerSharedHits;               
    unsigned int  m_numberOfBLayerOutliers;
     
    unsigned int  m_numberOfContribPixelLayers;
    unsigned int  m_numberOfPixelHits;                      
    unsigned int  m_numberOfPixelSharedHits;                
    unsigned int  m_numberOfPixelHoles;                     
    unsigned int  m_numberOfGangedPixels;
    unsigned int  m_numberOfGangedFlaggedFakes;                                                            
     
    unsigned int  m_numberOfSCTHits;                 
    unsigned int  m_numberOfSCTSharedHits;                  
    unsigned int  m_numberOfSCTHoles;                       
    unsigned int  m_numberOfSCTDoubleHoles;          
    unsigned int  m_numberOfTRTHits;                        
    unsigned int  m_numberOfTRTXenonHits;                        
    unsigned int  m_numberOfTRTHighThresholdHits;           
    unsigned int  m_numberOfTRTOutliers;                    
    unsigned int  m_numberOfTRTHighThresholdOutliers;                                                        
    unsigned int  m_numberOfOutliersOnTrack;         
     
    unsigned int  m_numberOfPixelOutliers;
    unsigned int  m_numberOfPixelDeadSensors;
    unsigned int  m_numberOfPixelSpoiltHits; 
    unsigned int  m_numberOfBlayersMissed;
     
    unsigned int  m_numberOfSCTOutliers;
    unsigned int  m_numberOfSCTDeadSensors;
    unsigned int  m_numberOfSCTSpoiltHits;   
    unsigned int  m_numberOfTRTHoles;        
    unsigned int  m_numberOfTRTDeadStraws;   
    unsigned int  m_numberOfTRTTubeHits;    
        
  }; // class TrackParticleCnvAlg
  
} // namespace xAODMaker

#endif // XAODCREATORALGS_TRACKPARTICLECREATOR_H
