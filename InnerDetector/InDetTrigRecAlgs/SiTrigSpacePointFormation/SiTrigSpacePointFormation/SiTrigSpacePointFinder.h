/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

//!<  Trigger includes
#include "TrigInterfaces/FexAlgo.h"

//!< INCLUDES
#include "Identifier/IdentifierHash.h"
// typedef, cannot fwd declare
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "IRegionSelector/IRegSelTool.h"

#include <string>
#include <vector>

//forward decl

class SpacePointCollection; 
class SpacePointContainer; 
class SpacePointOverlapCollection;

class TrigTimer;

namespace InDet{

  class SiSpacePointMakerTool;
  class ITrigSCT_SpacePointTool;
  
  class SiTrigSpacePointFinder: public HLT::FexAlgo {
  public:
    
    SiTrigSpacePointFinder(const std::string& name,
			   ISvcLocator* pSvcLocator);
    
    ~SiTrigSpacePointFinder();
    
    HLT::ErrorCode hltInitialize(); 
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    
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
    
    IdentifierHash m_maxKey;
    
    const SCT_ClusterContainer *m_sctClusterContainer{};
    const PixelClusterContainer *m_pixelClusterContainer{};
    SpacePointContainer* m_SpacePointContainerSCT; 
    SpacePointContainer* m_SpacePointContainerPixel; 

    SpacePointOverlapCollection*    m_spOverlapColl;     

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    SG::ReadCondHandleKey<InDet::SiElementPropertiesTable> m_SCTPropertiesKey{this, "SCTPropertiesKey",
        "SCT_ElementPropertiesTable", "Key of input SiElementPropertiesTable for SCT"};

    ToolHandle< ITrigSCT_SpacePointTool > m_trigSpacePointTool;
    ToolHandle< SiSpacePointMakerTool > m_SiSpacePointMakerTool;
    
    ToolHandle<IRegSelTool>     m_regionSelector_pixel { this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel" };     //!< region selector service
    ToolHandle<IRegSelTool>     m_regionSelector_sct   { this, "RegSelTool_SCT",   "RegSelTool/RegSelTool_SCT" };     //!< region selector service
 
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
