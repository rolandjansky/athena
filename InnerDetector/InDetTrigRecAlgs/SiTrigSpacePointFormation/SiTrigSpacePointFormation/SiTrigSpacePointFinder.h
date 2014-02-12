/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: SiTrigSpacePointFinder.h
//
// author: Patricia Conde Muino 26/01/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Trigger top Algorithm to form Si SpacePoints.
//               It receives the input from the TriggerElement,so it
//               asumes that other algorithms were run first.
//               The algorithm is based in the offline version
//               (SiTrackerSpacePointFinder)
//
// Modified :  Jiri Masik <Jiri.Masik@cern.ch>
//     added modules overlap handling
//       
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef SiTrigSpacePointFormation_SI_POINT_FINDER_H
#define SiTrigSpacePointFormation_SI_POINT_FINDER_H

//!< INCLUDES                                                    
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>

#include "Identifier/IdentifierHash.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

//!<  Trigger includes
#include "TrigInterfaces/FexAlgo.h"

//forward decl

class SpacePointCollection; 
class SpacePointContainer; 
class SpacePointOverlapCollection;
class IRegSelSvc;
class PixelID;
class TrigTimer;


namespace InDet{

  class SiElementPropertiesTable;
  class SiSpacePointMakerTool;
  class ITrigSCT_SpacePointTool;
  
  class SiTrigSpacePointFinder: public HLT::FexAlgo {
  public:
    
    SiTrigSpacePointFinder(const std::string& name,
			   ISvcLocator* pSvcLocator);
    
    ~SiTrigSpacePointFinder();
    
    HLT::ErrorCode hltBeginRun(); 
    HLT::ErrorCode hltInitialize(); 
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();
    
  private:
    
    //!< data members
    std::string m_spacePointsSCTName;
    std::string m_spacePointsPixelName;
    std::string m_pixelClustersName;
    std::string m_sctClustersName;
    std::string m_spOverlapName;

    bool m_selectPixels;
    bool m_selectSCTs;
    bool m_overlap;
    
    //    int m_numberOfEvents;
    //    const InDetDD::PixelDetectorManager *m_managerPixel; 
    //    const PixelID* m_idHelperPixel;
    IdentifierHash m_maxKey;
    
    const SCT_ClusterContainer *m_sctClusterContainer;
    const PixelClusterContainer *m_pixelClusterContainer;
    SpacePointContainer* m_SpacePointContainerSCT; 
    SpacePointContainer* m_SpacePointContainerPixel; 

    SpacePointOverlapCollection*    m_spOverlapColl;     

    ToolHandle< ITrigSCT_SpacePointTool > m_trigSpacePointTool;
    ToolHandle< SiSpacePointMakerTool > m_SiSpacePointMakerTool;
    
    ServiceHandle<IRegSelSvc>     m_regionSelector;     //!< region selector service
    bool m_doFullScan;		//!< skip RegionSelector and indefFind for FullScan
    
    double                  m_etaHalfWidth;          //!< ROI half-width in eta
    double                  m_phiHalfWidth;          //!< ROI half-width in phi
    
    int m_nSCT;
    int m_nPix;
    int m_nReceivedClustersSCT;
    int m_nReceivedClustersPix;
    std::vector<int> m_SPindentifierSct;
    std::vector<int> m_SPindentifierPix;
    
    std::vector<IdentifierHash> m_listOfPixIds;
    std::vector<IdentifierHash> m_listOfSctIds;
    
    // Timing
    TrigTimer *m_timerRegSel;
    TrigTimer *m_timerSeed;
    TrigTimer *m_timerRtrSi;
    TrigTimer *m_timerSGate;
    TrigTimer *m_timerRtrPx;
    TrigTimer *m_timerFindSi;
    TrigTimer *m_timerFindPx;
    
  };
  
}

#endif // SiSpacePointFormation_SI_POINT_FINDER_H
