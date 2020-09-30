/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: TRT_TrgRIO_Maker.h
 //
 // author: Patricia Conde Muino 25/01/2005
 //         Patricia.Conde.Muino@cern.ch
 //
 // Description:  Trigger top Algorithm to form TRT RIO's.
 //               It receives the input from the RegionSelector.
 //               The algorithm is based in the offline version
 //               (TRT_RIO_Maker)
 //
 // Modified :
 //         Jiri Masik
 //
 // -------------------------------
 // ATLAS Collaboration
 */
////////////////////////////////////////////////////////////////////////////
#ifndef TRT_TRGRIO_MAKER_H
#define TRT_TRGRIO_MAKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

//!< Trigger 
#include "TrigInterfaces/FexAlgo.h"

//typedef - cannot be declared forward
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "IRegionSelector/IRegSelTool.h"

class TrigTimer;
class ITRT_ByteStream_ConditionsSvc;
class IROBDataProviderSvc;

namespace InDet{

  class ITRT_DriftCircleTool;
  class ITrigRawDataProviderTool;

  class TRT_TrgRIO_Maker : public HLT::FexAlgo {
  
    ///////////////////////////////////////////////////////////////////
    //!< Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    
    TRT_TrgRIO_Maker(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TRT_TrgRIO_Maker();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize   ()  ;
  
    //Method to prepare ROB ID list
    using HLT::FexAlgo::prepareRobRequests;
    HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE);


    // /////////////////////////////////////////////////////////////////
    //!< Private data
    // /////////////////////////////////////////////////////////////////
  private:
  
    //monitoring
    int m_numTrtDriftCircles;
    int m_numTrtIds;
  
    std::vector<IdentifierHash> m_listOfTrtIds; 
    std::vector<int> m_ClusHashId;
    std::vector<int> m_TrtBSErr;


    std::string                    m_trt_rdo_location   ;
    std::string                    m_trt_rio_location   ;

    ToolHandle<ITrigRawDataProviderTool>   m_rawDataProvider;
    ToolHandle< ITRT_DriftCircleTool > m_driftcircle_tool   ;
    std::string                    m_diftCircleToolName; //!< drift circle's rec algorithm
    InDet::TRT_DriftCircleContainer* m_riocontainer       ;
    int                            m_mode_rio_production;

    ToolHandle<IRegSelTool>    m_regionSelector { this, "RegSelTool", "RegSelTool/RegSelTool_TRT" }; //!<  region selector service
    ServiceHandle<ITRT_ByteStream_ConditionsSvc> m_bsErrorSvc;
    ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;   //!< ROB Data Provide Service
    
    bool m_doFullScan;            //!< support for FullScan

    double                   m_etaHalfWidth;       //!< ROI half-width in eta.
    double                   m_phiHalfWidth;       //!< ROI half-width in phi.
    bool                     m_doTimeOutChecks;    //check global timer

    // Timing
    TrigTimer   *m_timerRegSel; 
    TrigTimer   *m_timerSeed;   
    TrigTimer   *m_timerCluster;
    TrigTimer   *m_timerSGate;  

    static const int EFID_MAXNUM_TRT_BS_ERRORS=6;

  };
}
#endif // TRT_TRGRIO_MAKER_H
