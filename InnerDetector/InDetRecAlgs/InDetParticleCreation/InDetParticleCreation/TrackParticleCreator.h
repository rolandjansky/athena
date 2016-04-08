/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                      TrackParticleCreator.h  -  Description
                             -------------------
    begin   : 27-02-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
 
 ***************************************************************************/

#ifndef INDETTRACKPARTICLECREATION_PARTICLECREATOR_H
#define INDETTRACKPARTICLECREATION_PARTICLECREATOR_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk
{
  class ITrackParticleCreatorTool;
  class IPRD_AssociationTool;
}

namespace InDet
{
  class TrackParticleCreator : public AthAlgorithm
  {
  public:
    TrackParticleCreator(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrackParticleCreator();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:
    ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
    ToolHandle< Trk::IPRD_AssociationTool > m_assoTool; /** used to work out which (if any) PRDs are shared between tracks*/

    std::string m_tracksName;
    std::string m_vxCandidatesPrimaryName;
    std::string m_trackParticleOutputName;
    bool m_doSharedHits;
    bool m_addAllTracksToDummyVtx;

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
    unsigned int  m_numberOfGangedFlaggedFakes                                                            ;
                                                     
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
    
  };
}

#endif
