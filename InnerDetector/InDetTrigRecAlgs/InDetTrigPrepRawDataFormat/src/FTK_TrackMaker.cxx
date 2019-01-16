/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for FTK_TrackMaker.
//****************************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "AthenaKernel/Timeout.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetTrigPrepRawDataFormat/FTK_TrackMaker.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h" 


//Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

namespace InDet{

  //----------------------------------------------------------------------------
  FTK_TrackMaker::FTK_TrackMaker 
  (const std::string& name,ISvcLocator* pSvcLocator) : 
    HLT::FexAlgo(name,pSvcLocator),
    m_regionSelector("RegSelSvc", name),
    //    m_bsErrorSvc("TrigFTKBytestreamErrorSvc",name),
    m_ftkdatasvc("TrigFTK_DataProviderSvc",name),
    m_robDataProvider("ROBDataProviderSvc", name)
  {
    
  
    m_timerDPS=nullptr;
  }
  

  //----------------------------------------------------------------------------
  HLT::ErrorCode FTK_TrackMaker::hltInitialize() {
      
    // Retrieving Region Selector Tool:
    if ( m_regionSelector.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_regionSelector.propertyName()
		     << " : Unable to retrieve RegionSelector tool "
		     << m_regionSelector.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    /*
    if (m_bsErrorSvc.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_bsErrorSvc );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    */
    
    //retrieve rob data provider service
    if (m_robDataProvider.retrieve().isFailure()){
      ATH_MSG_FATAL( "Failed to retrieve " << m_robDataProvider );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else
      ATH_MSG_INFO( "Retrieved service " << m_robDataProvider << " in FTK_TrackMaker." );

    if(m_ftkdatasvc.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_ftkdatasvc );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    
    // Timers:
    m_timerDPS  = addTimer("DataProviderSvc"); // 1

    return HLT::OK;
  }

  //----------------------------------------------------------------------------
  FTK_TrackMaker::~FTK_TrackMaker() {}  


  //----------------------------------------------------------------------------
  HLT::ErrorCode FTK_TrackMaker::hltExecute(const HLT::TriggerElement*,
					      HLT::TriggerElement* outputTE){
    
    //initialisation of monitored quantities
  

    //reset BS error service
    //m_bsErrorSvc->resetCounts();
    StatusCode scdec = StatusCode::SUCCESS;

    
    // Get RoiDescriptor
    const TrigRoiDescriptor* roi;
    if ( HLT::OK !=  getFeature(outputTE, roi) ) {
      ATH_MSG_WARNING( "Can't get RoI" );
      return HLT::NAV_ERROR;
    }
    
    if (!roi){
      ATH_MSG_WARNING ( "Received NULL RoI" );
      return HLT::NAV_ERROR;
    }
    

    if(doTiming()) m_timerDPS->start();
    
    //    scdec = 
    //m_ftkdatasvc->
    TrackCollection* ftkTracks = m_ftkdatasvc->getTracksInRoi(*roi, false /* =ftkrefit */ );
    if (ftkTracks)
      ATH_MSG_DEBUG( "REGTEST: decoded " << ftkTracks->size() << " tracks "); 

    if(doTiming()) m_timerDPS->stop();


    //initial debugging - can be removed later
    std::vector<IdentifierHash> listOfIds;
    m_regionSelector->DetHashIDList( FTK, *roi, listOfIds);
    ATH_MSG_DEBUG( "REGTEST: FTK: Roi contains " 
		   << listOfIds << " det. Elements" );

    
    if (scdec.isSuccess()){
      //check for recoverable errors
      int n_err_total = 0;
       
      /*
      int bsErrors[EFID_MAXNUM_FTK_BS_ERRORS];
      for (size_t idx = 0; idx<size_t(EFID_MAXNUM_FTK_BS_ERRORS); idx++){
	int n_errors = m_bsErrorSvc->getNumberOfErrors(FTKByteStreamErrors::errorTypes(idx));
	n_err_total += n_errors;
	bsErrors[idx] = n_errors;
      }
      */


      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "decoding errors: " << n_err_total;

      /*
      if (n_err_total){
	for (size_t idx = 0; idx<size_t(EFID_MAXNUM_FTK_BS_ERRORS); idx++){
	  //	  m_FtkBSErr.push_back(bsErrors[idx]);
	  if (bsErrors[idx])
	    m_FtkBSErr.push_back(idx);
	  if(msgLvl(MSG::DEBUG))
	    msg(MSG::DEBUG) << " " << bsErrors[idx];
	}
      }
      ATH_MSG_DEBUG( "" );
      */
    }
    else {
      ATH_MSG_ERROR( "FTK decoding failed failed" );
    }

  

    return HLT::OK;
  }

  //----------------------------------------------------------------------------
  HLT::ErrorCode FTK_TrackMaker::hltFinalize() {

    return HLT::OK;
  }

  //---------------------------------------------------------------------------
  HLT::ErrorCode FTK_TrackMaker::prepareRobRequests(const HLT::TriggerElement* inputTE){

    ATH_MSG_DEBUG( "FTK_TrackMaker::prepareRobRequests()" );

    //Calculate ROBs needed - this code should be shared with hltExecute to avoid slightly different requests
    const TrigRoiDescriptor* roi = nullptr;

    if (getFeature(inputTE, roi) != HLT::OK || roi == nullptr){
      ATH_MSG_WARNING( "REGTEST / Failed to find RoiDescriptor" );
      return HLT::NAV_ERROR;
    }

    ATH_MSG_DEBUG( "REGTEST prepareROBs / event RoI ID " << roi->roiId()
		   << " located at   phi = " << roi->phi()
		   << ", eta = " << roi->eta() );


    std::vector<unsigned int> uIntListOfRobs;
    m_regionSelector->DetROBIDListUint( FTK, *roi, uIntListOfRobs );

    ATH_MSG_DEBUG( "list of pre-registered ROB ID in FTK: ");
    for(auto i : uIntListOfRobs) {
      ATH_MSG_DEBUG( i );
    }

    //m_robDataProvider->addROBData( uIntListOfRobs );
    config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
    uIntListOfRobs.clear();

    return HLT::OK;
    
  }

  //---------------------------------------------------------------------------
}
