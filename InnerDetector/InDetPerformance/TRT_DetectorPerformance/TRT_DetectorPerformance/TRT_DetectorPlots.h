/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DetectorPerformance_TRT_DetectorPlots_H
#define TRT_DetectorPerformance_TRT_DetectorPlots_H

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"

#include "DataModel/DataVector.h"

#include "EventInfo/EventInfo.h"

#include "Identifier/Identifier.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetRawData/TRT_RDO_Container.h"


namespace InDetDD { 
  class TRT_DetectorManager;
  class SCT_DetectorManager;
  class PixelDetectorManager;
}
class TRT_ID;
class SCT_ID;
class PixelID;
class ITRT_CalDbSvc;
class ITRT_StrawNeighbourSvc;
namespace InDet {
  class TRT_StrawMap;
}

namespace Trk {
  class Segment;
  class Track;
}
class PRD_MultiTruthCollection;

class ITHistSvc;
class TTree;


/////////////////////////////////////////////////////////////////////////////

namespace InDet
{
  /**
   * TRT_DetectorPlots: Algorithm to produce an ntuple containing track, track segment, 
   * and hit information used in viewing the detector performance
  */
  class TRT_DetectorPlots : public AthAlgorithm {

  public:
  
    TRT_DetectorPlots (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  protected:
  
    int m_doMC;
    int m_doRDO;
    int m_numNoise;
  
    int m_doSegments;
    std::string m_barrelSegmentsName;         
    std::string m_endcapSegmentsName;         	

    std::string m_tracksName;         
    std::string m_fileName;         
  
  private:

    StatusCode load(); // load private members from StoreGate
    
    
    void fillEventAndTrackTrees(const EventInfo *eventInfo);
    void fillSegmentTree();
    void fillHitsTree();
    
    /* print 8BC time information form pixel hits */
    void pixelRDOhits(int runNumber);
    void RDOhits();
    void DriftCircleHits();
	
    void makeSegments();
    void MC();
    
    ServiceHandle<ITHistSvc> m_histSvc;
    TTree* m_treeEvent;
    TTree* m_treeTrack;
    TTree* m_treeSegment;
    TTree* m_treeHits;
    TTree* m_treePixel;
  
    ///////////////////////
    /// Track Parameters
    ///////////////////////

    int m_eventNumberOfTracks;
    int m_eventNumberOfHitsOnTracks[7];
    int m_eventL1Type;
    int m_eventL1TagName;
    double m_eventTrackInfo[5];
    double m_eventPixelTimeBin;
    std::vector<double>* m_eventPixelX;
    std::vector<double>* m_eventPixelY;
    std::vector<double>* m_eventPixelZ;
    std::vector<int>* m_eventPixelHitTimeBin;
    std::vector<double>* m_eventSCTX;
    std::vector<double>* m_eventSCTY;
    std::vector<double>* m_eventSCTZ;
    std::vector<int>* m_eventSCTHitTimeBin;	
    std::vector<double>* m_eventTRTX;
    std::vector<double>* m_eventTRTY;
    std::vector<double>* m_eventTRTZ;
    int m_eventL2Alg[3]; // 1: SiTrack, 2: IdScan, 3: TRTSegFinder
    int m_siSP[2]; // N si  upper, 

    int m_trackRunNumber;
    int m_trackEventNumber;
    int m_trackTrigL1Type;
    int m_trackTrigStreamSize;      // number of trigger streams
    int m_trackTrig1MU1;            // TGC trigger
    int m_trackTrig1MU2;            // RPC trigger - non-pointing
    int m_trackTrig1MU3;            // RPC trigger - pointing
    int m_trackTrigL1Calo;          // Calo trigger
    int m_trackTrigNIM0;            // Tile trigger
    int m_trackTrigNIM4;            // ID scintillator trigger
    int m_trackTrigMBTS;            // min bias trigger
    int m_trackTrigEML1Calo; 
    int m_trackTrigHLTerror;
    int m_trackTrigAllTeIDSelected; // LVL 2 trigger
    int m_trackTrigAllTePixel;      // L2 pixel volume
    int m_trackNumTracks;
    int m_trackNumTrtTracks;
    int m_trackNumSctHits;
    int m_trackNumTrtHits;
    int m_trackNumTrtHitsBarrelA;
    int m_trackNumTrtHitsBarrelC;
    int m_trackNumTrtHitsEcA;
    int m_trackNumTrtHitsEcC;
    int m_trackNdf;
    double m_trackChi2;
    double m_trackTheta;
    double m_trackD0;
    double m_trackZ0;
    double m_trackPhi0;
    double m_trackEventPhase;

    ///////////////////////////////////////
    /// Hit parameters for hits on track
    ///////////////////////////////////////
    std::vector<int>* m_trackSide;
    std::vector<int>* m_trackLayerOrWheel;
    std::vector<int>* m_trackPhi; // phi module or stack number 
    std::vector<int>* m_trackStrawLayer;
    std::vector<int>* m_trackStraw;
    std::vector<int>* m_trackStrawNum;
    std::vector<int>* m_trackChip;
    std::vector<int>* m_trackBoard;
    std::vector<int>* m_trackDriftTimeBin;
    std::vector<int>* m_trackTrailingEdge;
    std::vector<int>* m_trackHighThreshold;
    std::vector<int>* m_trackDriftCircleWord;
    std::vector<double>* m_trackX;
    std::vector<double>* m_trackY;
    std::vector<double>* m_trackZ;
    std::vector<double>* m_trackT0;

    std::vector<double>* m_trackTrackR;
    std::vector<double>* m_trackLocR;
    std::vector<double>* m_trackErrR;

    //////////////////////////////////
    /// Hit parameters for all hits
    //////////////////////////////////
    std::vector<int>* m_segmentSide;
    std::vector<int>* m_segmentLayerOrWheel;
    std::vector<int>* m_segmentPhi; // phi module or stack number 
    std::vector<int>* m_segmentStrawLayer;
    std::vector<int>* m_segmentStraw;
    std::vector<int>* m_segmentStrawNum;
    std::vector<int>* m_segmentChip;
    std::vector<int>* m_segmentBoard;
    std::vector<int>* m_segmentDriftTimeBin;
    std::vector<int>* m_segmentTrailingEdge;
    std::vector<int>* m_segmentDriftCircleWord;
    std::vector<int>* m_segmentHitOnTrack;
    std::vector<double>* m_segmentX;
    std::vector<double>* m_segmentY;
    std::vector<double>* m_segmentZ;
    std::vector<double>* m_segmentT0;

    //////////////////////////////////
    /// Hit parameters for all hits
    //////////////////////////////////
    int m_hitsSide;
    int m_hitsLayerOrWheel;
    int m_hitsPhi; // phi module or stack number 
    int m_hitsStrawLayer;
    int m_hitsStraw;
    int m_hitsStrawNum;
    int m_hitsChip;
    int m_hitsBoard;
    int m_hitsDriftTimeBin;
    int m_hitsTrailingEdge;
    int m_hitsDriftCircleWord;
    int m_hitsHitOnTrack;
    double m_hitsX;
    double m_hitsY;
    double m_hitsZ;
    double m_hitsT0;

    //////////////////////////////////
    /// Pixel related
    //////////////////////////////////

    int m_pixelNumberOfPixelHits;
    int m_pixelMaxNumberOfTRTBarrelHitsOnTrack;
    double m_pixelTRTtrackD0;
    double m_pixelTRTtrackPhi0;
    double m_pixelTRTtrackEP;
    double m_pixelComTime;
    std::vector<int>* m_pixelL1TimeBin;
	
    std::vector<Identifier>* m_onTrackIdentifiers; // used in selecting hits on track
      
    const InDetDD::TRT_DetectorManager *m_trtDetMgr;
    const TRT_ID *m_trtHelper;
    const InDetDD::SCT_DetectorManager *m_SCTDetMgr;
    const SCT_ID *m_SCTHelper;
    const InDetDD::PixelDetectorManager *m_PixelDetMgr;
    const PixelID *m_PixelHelper;

    ServiceHandle<ITRT_CalDbSvc>          m_trtCondDbTool;
    ServiceHandle<ITRT_StrawNeighbourSvc> m_trtStrawNeighbourTool;
    ToolHandle<InDet::TRT_StrawMap>   m_trtStrawMap;

    const TRT_RDO_Container*               m_rdoContainer;
    const InDet::TRT_DriftCircleContainer* m_trtDriftCircleContainer;
	
    const DataVector<Trk::Segment>*   	   m_segmentCollectionBarrel; 
    const DataVector<Trk::Segment>*   	   m_segmentCollectionEc; 
    const DataVector<Trk::Track>*     	   m_trkCollection;
    const PRD_MultiTruthCollection*   	   m_truthTrt;

    int m_runNumber;	
  
  };

} // end namespace

#endif
