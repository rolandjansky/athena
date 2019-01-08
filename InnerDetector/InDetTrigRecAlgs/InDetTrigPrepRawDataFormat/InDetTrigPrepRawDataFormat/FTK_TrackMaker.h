/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: FTK_TrackMaker.h
 //
 // author: Jiri.Masik@manchester.ac.uk
 //         
 // Description: decode FTK data 
 //
 // -------------------------------
 // ATLAS Collaboration
 */
////////////////////////////////////////////////////////////////////////////
#ifndef FTK_TRACK_MAKER_H
#define FTK_TRACK_MAKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

//!< Trigger 
#include "TrigInterfaces/FexAlgo.h"


class IRegSelSvc;
class TrigTimer;
class IFTK_DataProviderSvc;
class IROBDataProviderSvc;


namespace InDet{


  class FTK_TrackMaker : public HLT::FexAlgo {
  
    ///////////////////////////////////////////////////////////////////
    //!< Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    
    FTK_TrackMaker(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~FTK_TrackMaker();
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
    

    ServiceHandle<IRegSelSvc>                  m_regionSelector; //!<  region selector service
    ServiceHandle<IFTK_DataProviderSvc>        m_ftkdatasvc;
    //    ServiceHandle<IFTK_ByteStreamErrorSvc>     m_bsErrorSvc;
    ServiceHandle<IROBDataProviderSvc>         m_robDataProvider;   //!< ROB Data Provide Service
    
    // Timing
    TrigTimer   *m_timerDPS; 

    static const int EFID_MAXNUM_FTK_BS_ERRORS=6;

  };
}
#endif // FTK_TRACK_MAKER_H
