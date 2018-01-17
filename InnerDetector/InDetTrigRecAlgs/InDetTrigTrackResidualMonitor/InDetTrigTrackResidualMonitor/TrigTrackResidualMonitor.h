/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: TrigTrackResidualMonitor.h
 //
 // author: Vikash Chavda,Jiri Masik 
 //
 // Description:  produce Residuals and Pulls for input track collection 
 */
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGTRACKSLIMMER_H
#define INDETTRIGTRACKSLIMMER_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/ITHistSvc.h"


//typedef
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackInfo.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkValEvent/TrackStateData.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"  
#include "InDetReadoutGeometry/PixelDetectorManager.h"  
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"


class AtlasDetectorID;
class Track;
class IInDetConditionsSvc;
class PixelID;
class SCT_ID;

class SvcLocator;
class StoreGateSvc;
class SCT_NeighboursTable;

namespace InDetDD{
  class SCT_DetectorManager;
  class PixelDetectorManager;
}

class IInDetAlignHitQualSelTool; 

namespace Trk {

  class IUpdator;
  class IPropagator;
  
  class ITrackSlimmingTool;
  class ITrackSelectorTool;
  class ITrackSummaryTool;

  class MeasurementBase;
  class IResidualPullCalculator;
  class TrackStateOnSurface;
  class ResidualPull;
  class AlignTSOS;
  class Surface;
  class RIO_OnTrack;


}

namespace InDet
{

  class TrigTrackResidualMonitor : public HLT::FexAlgo {

    
 
  public:
    TrigTrackResidualMonitor(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigTrackResidualMonitor();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();
    
    
  private:
    const TrackCollection*                m_allTracksFromStoreGate;


    // tools    
    
    const AtlasDetectorID*   m_idHelper;
    
    const PixelID*           m_idHelperPixel;
    const SCT_ID*            m_idHelperSCT;
    
 
    

    // monitoring items
    bool                     m_monitoring; // monitoring variables
    std::string              m_path;       // monitoring path
    
    
    ToolHandle< Trk::ITrackSelectorTool > m_trackSelectorTool;
    ToolHandle< Trk::ITrackSummaryTool>   m_trkSummaryTool;
    
    ToolHandle<Trk::IUpdator>             m_updator;
    ToolHandle<Trk::IPropagator>          m_propagator; 
    
    Trk::ParticleHypothesis   m_ParticleHypothesis;         
    
    // The residual and pull calculator tool
    ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator;
    
    
    const InDetDD::SCT_DetectorManager    *m_SCT_Manager;
    const InDetDD::PixelDetectorManager   *m_Pixel_Manager;



    ///// Biased Residuals and Pulls

    
    //Barrel region
    std::vector<double> m_resPixellocXBiasedBarrel;
    std::vector<double> m_resPixellocYBiasedBarrel;
    std::vector<double> m_pullPixellocXBiasedBarrel;
    std::vector<double> m_pullPixellocYBiasedBarrel;
    std::vector<double> m_resSCTBiasedBarrel;
    std::vector<double> m_pullSCTBiasedBarrel;
    

    //End Cap A
    std::vector<double> m_resPixellocXBiasedEndCapA;
    std::vector<double> m_resPixellocYBiasedEndCapA;
    std::vector<double> m_pullPixellocXBiasedEndCapA;
    std::vector<double> m_pullPixellocYBiasedEndCapA;
    std::vector<double> m_resSCTBiasedEndCapA;
    std::vector<double> m_pullSCTBiasedEndCapA;
    

    //End Cap C
    std::vector<double> m_resPixellocXBiasedEndCapC;
    std::vector<double> m_resPixellocYBiasedEndCapC;
    std::vector<double> m_pullPixellocXBiasedEndCapC;
    std::vector<double> m_pullPixellocYBiasedEndCapC;
    std::vector<double> m_resSCTBiasedEndCapC;
    std::vector<double> m_pullSCTBiasedEndCapC;
    

    //////// Unbiased Residuals and Pulls


    //Barrel region
    std::vector<double> m_resPixellocXUnbiasedBarrel;
    std::vector<double> m_resPixellocYUnbiasedBarrel;
    std::vector<double> m_pullPixellocXUnbiasedBarrel;
    std::vector<double> m_pullPixellocYUnbiasedBarrel;
    std::vector<double> m_resSCTUnbiasedBarrel;
    std::vector<double> m_pullSCTUnbiasedBarrel;
    
    
    //End Cap A
    std::vector<double> m_resPixellocXUnbiasedEndCapA;
    std::vector<double> m_resPixellocYUnbiasedEndCapA;
    std::vector<double> m_pullPixellocXUnbiasedEndCapA;
    std::vector<double> m_pullPixellocYUnbiasedEndCapA;
    std::vector<double> m_resSCTUnbiasedEndCapA;
    std::vector<double> m_pullSCTUnbiasedEndCapA;


    //End Cap C
    std::vector<double> m_resPixellocXUnbiasedEndCapC;
    std::vector<double> m_resPixellocYUnbiasedEndCapC;
    std::vector<double> m_pullPixellocXUnbiasedEndCapC;
    std::vector<double> m_pullPixellocYUnbiasedEndCapC;
    std::vector<double> m_resSCTUnbiasedEndCapC;
    std::vector<double> m_pullSCTUnbiasedEndCapC;

    
    
    //Variables, nhits, pt
    std::vector<double> m_TrackPt;
    std::vector<double> m_npix;
    std::vector<double> m_nsct;
    std::vector<double> m_npixh;
    std::vector<double> m_nscth;
    

   
    };
  
}

#endif
