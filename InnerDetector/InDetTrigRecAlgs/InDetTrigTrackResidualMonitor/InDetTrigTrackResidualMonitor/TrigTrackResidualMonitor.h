/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#ifndef TrigTrackResidualMonitor_H
#define TrigTrackResidualMonitor_H

#include "GaudiKernel/ToolHandle.h"

//typedef
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackInfo.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

class AtlasDetectorID;
class Track;
class PixelID;
class SCT_ID;

class SvcLocator;
class SCT_NeighboursTable;

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
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    
    
  private:
    const TrackCollection*                m_allTracksFromStoreGate;


    // tools    
    
    const AtlasDetectorID*   m_idHelper{};
    
    const PixelID*           m_idHelperPixel{};
    const SCT_ID*            m_idHelperSCT{};
    
 
    

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
