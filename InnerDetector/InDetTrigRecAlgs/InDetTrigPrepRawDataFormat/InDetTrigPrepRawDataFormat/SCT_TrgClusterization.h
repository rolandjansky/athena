/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: SCT_TrgClusterization.h
//
// author: Patricia Conde Muino 17/01/2005
//         Patricia.Conde.Muino@cern.ch
//         Jiri Masik <Jiri.Masik@hep.manchester.ac.uk>
//
// Description:  Trigger top Algorithm to clusterize the SCT RDOs.
//               It receives the input from the RegionSelector.
//               The algorithm is based in the offline version
//               (SCT_Clusterization)
//
// Modified : 
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef SCT_TRGCLUSTERIZATION_H
#define SCT_TRGCLUSTERIZATION_H

//!<  Base class
#include "TrigInterfaces/FexAlgo.h"

#include <string>
#include <set>

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#include "Identifier/IdentifierHash.h"
#include "IRegionSelector/IRegSelTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"
#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"

//typedefs - cannot be declared forward
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class TrigTimer;

class SCT_ID;
class SCT_ChannelStatusAlg;

namespace InDet {
    
  class SCT_TrgClusterization : public HLT::FexAlgo {
    
    ///////////////////////////////////////////////////////////////////
    //!<  Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    
    //!<  Constructor with parameters:
    SCT_TrgClusterization(const std::string &name,ISvcLocator *pSvcLocator);
    
    //!<  Basic algorithm methods:
    virtual HLT::ErrorCode hltInitialize();
    virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
				      HLT::TriggerElement* output);
    virtual HLT::ErrorCode hltFinalize();

    // Method to prepare ROB id list
    using HLT::FexAlgo::prepareRobRequests;
    HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE);
    
    ///////////////////////////////////////////////////////////////////
    //!<  Private methods:
    ///////////////////////////////////////////////////////////////////
  private:
    SCT_TrgClusterization();
    SCT_TrgClusterization(const SCT_TrgClusterization&);
    SCT_TrgClusterization &operator=(const SCT_TrgClusterization&);
    
    ///////////////////////////////////////////////////////////////////
    //!<  Private data members:
    ///////////////////////////////////////////////////////////////////
  private:
    
    //monitoring
    int m_numSctClusters;
    int m_numSctIds;
    
    std::vector<IdentifierHash> m_listOfSctIds; 
    SCT_ClusterCollection* m_clusterCollection;
    std::vector<int> m_ClusHashId;
    std::vector<int> m_SctBSErr;
    std::vector<int> m_occupancyHashId;

    
    ToolHandle<ITrigRawDataProviderTool>    m_rawDataProvider;
    ToolHandle<ISCT_ClusteringTool> m_clusteringTool; //!<  clustering algorithm
    std::string             m_clustersName; 
    std::string             m_flaggedCondDataName;
    const SCT_ID*           m_idHelper;
    
    SCT_ClusterContainer*   m_clusterContainer;
    
    // !<  Trigger part
    ToolHandle<IRegSelTool>    m_regionSelector{ this, "RegSelTool", "RegSelTool/RegSelTool_SCT" }; //!<  region selector service
    bool m_doFullScan;                       //!<  support for FullScan mode
    double                  m_etaHalfWidth;          //!<  ROI half-width in eta.
    double                  m_phiHalfWidth;          //!<  ROI half-width in phi.
    std::string             m_sctRDOContainerName; 
    ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;   //!< ROB Data Provide Service

    //conditions
    ToolHandle<IInDetConditionsTool>         m_pSummaryTool{this, "conditionsSummaryTool",
        "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT conditions summary"};
    bool                                     m_checkBadModules;
    unsigned int                             m_maxRDOs;
    std::set<IdentifierHash>                 m_flaggedModules;
    bool                   m_doTimeOutChecks;   //check global timer

    // For P->T converter of SCT_Clusters
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    
    // Timing
    TrigTimer*  m_timerSGate;      
    TrigTimer*  m_timerCluster;
    TrigTimer*  m_timerRegSel;      
    TrigTimer*  m_timerDecoder;      

};

}

#endif // SCT_TRGCLUSTERIZATION_H
