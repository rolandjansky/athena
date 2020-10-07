/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: Pixel_TrgClusterization.h
//
// author: Patricia Conde Muino 24/01/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Trigger top Algorithm to clusterize the Pixel RDOs.
//               It receives the input from the RegionSelector.
//               The algorithm is based in the offline version
//               (PixelClusterization)
//
// Modified :
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////


#ifndef PIXEL_TRGCLUSTERIZATION_H
#define PIXEL_TRGCLUSTERIZATION_H

//!< Base class
#include "TrigInterfaces/FexAlgo.h"

//!< ID specific stuff
//typedefs cannot be declared forward 
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "Identifier/IdentifierHash.h"

#include "PixelConditionsTools/IPixelByteStreamErrorsTool.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"


#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "IRegionSelector/IRegSelTool.h"


class TrigTimer;
class PixelID;
class IROBDataProviderSvc;

namespace InDet {
  
  class ITrigRawDataProviderTool;
  
  class Pixel_TrgClusterization : public HLT::FexAlgo {
    
    ///////////////////////////////////////////////////////////////////
    //!< Public methods:
    ///////////////////////////////////////////////////////////////////
  public:    
    
    // Constructor with parameters:
    Pixel_TrgClusterization(const std::string &name,ISvcLocator *pSvcLocator);
    
    
    ///////////////////////////////////////////////////////////////////
    //!< Non-const methods:
    ///////////////////////////////////////////////////////////////////
    
    // Basic algorithm methods:
    virtual HLT::ErrorCode hltInitialize();
    virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
				      HLT::TriggerElement* output);
    virtual HLT::ErrorCode hltFinalize();

    // Method to prepare ROB id list
    using HLT::FexAlgo::prepareRobRequests;
    HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE);
    
    ///////////////////////////////////////////////////////////////////
    //!< Private methods:
    ///////////////////////////////////////////////////////////////////
  private:
    Pixel_TrgClusterization();
    Pixel_TrgClusterization(const Pixel_TrgClusterization&);
    Pixel_TrgClusterization &operator=(const Pixel_TrgClusterization&);

    ///////////////////////////////////////////////////////////////////
    //!< Private data members:
    ///////////////////////////////////////////////////////////////////
  private:
    
    //monitoring
    int m_numPixClusters;
    int m_numPixIds;
    
    std::vector<IdentifierHash> m_listOfPixIds; 
    PixelClusterCollection* m_clusterCollection;
    
    std::vector<int> m_ClusHashId;
    std::vector<int> m_PixBSErr;
    
    ToolHandle<ITrigRawDataProviderTool>     m_rawDataProvider;
    ToolHandle<PixelGangedAmbiguitiesFinder> m_gangedAmbiguitiesFinder; 
    //!< class to find out which
    //!<  clusters shares ganged pixels
    ToolHandle<IPixelClusteringTool> m_clusteringTool;   //!< clustering algorithm
    
    std::string              m_pixelRDOContainerName; //!< RDO container name
    std::string              m_elementsObjectName; 
    std::string              m_clustersName; 
    std::string              m_ambiguitiesMapName;
    
    const PixelID*           m_idHelper{};
    PixelClusterContainer*   m_clusterContainer{};
    
    ToolHandle<IRegSelTool>  m_regionSelector{ this, "RegSelTool", "RegSelTool/RegSelTool_Pixel" };     //!< region selector service
    bool m_doFullScan;             //!< support for FullScan mode
    double                   m_etaHalfWidth;       //!< ROI half-width in eta
    double                   m_phiHalfWidth;       //!< ROI half-width in phi
    ToolHandle<IPixelByteStreamErrorsTool> m_bsError
    {this, "PixelByteStreamErrorsTool", "PixelByteStreamErrorsTool", "Tool for PixelByteStreamError"};

    ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;   //!< ROB Data Provide Service
    bool                     m_doTimeOutChecks;   //check global timer
    bool                     m_skipBSDecoding;    //option to skip BS decoding in MC
    
    // Timing
    TrigTimer   *m_timerRegSel;
    TrigTimer   *m_timerDecoder;
    TrigTimer   *m_timerCluster;
    TrigTimer   *m_timerSGate;

  };
  
}//end of ns

#endif // PIXEL_TRGCLUSTERIZATION_H
