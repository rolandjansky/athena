/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigTRT_DriftCircleProviderTool
// ( see header-file for details )
// -------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <algorithm>

// EDM & Identifier
#include "InDetIdentifier/TRT_ID.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"

// D.C. creation

#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"

#include "TrigOfflineDriftCircleTool/TrigTRT_DriftCircleProviderTool.h"
#include "TrigOfflineDriftCircleTool/TrigTRT_DCCollByteStreamTool.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"


#define MAXNUM_TRT_BS_ERRORS_3  6

typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ; 

TrigTRT_DriftCircleProviderTool::TrigTRT_DriftCircleProviderTool(const std::string& t, 
							   const std::string& n,
					 		   const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_regionSelector("RegSelSvc",n),
  m_robDataProvider("ROBDataProviderSvc",n),
  m_TRT_DCCollBSTool("TrigTRT_DCCollByteStreamTool"),
  m_trtByteStreamErrorService("TRT_ByteStream_ConditionsSvc", this->name())

{
  declareInterface< ITrigTRT_DriftCircleProviderTool >( this );
  declareProperty( "RegionSelectorToolName", m_regionSelectorToolName = "RegSelSvc" );  
  declareProperty( "ROBDataProviderSvcName", m_ROBDataProviderSvcName = "ROBDataProviderSvc" );
  declareProperty( "TRT_DCCollBSToolName", m_TRT_DCCollBSTool);
  declareProperty( "TRT_RoBlist", m_uIntListOfTRT_Robs);
  //  declareProperty( "zMin", m_zMin=-168.0 );
  // declareProperty( "zMax", m_zMax=-168.0 );
  
}

StatusCode TrigTRT_DriftCircleProviderTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}
 

StatusCode TrigTRT_DriftCircleProviderTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();

#ifndef NDEBUG
  msg(MSG::DEBUG) << "TrigTRT_DriftCircleProviderTool: in initialize" << endreq;
  msg(MSG::INFO) << "RegionSelectorToolName: " << m_regionSelectorToolName << endreq;
#endif

//   Get the region selector tool
//
  sc= m_regionSelector.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::FATAL) 
	      << "Unable to retrieve RegionSelector " << m_regionSelector << endreq;
    return sc;
  };


  sc= m_TRT_DCCollBSTool.retrieve();
  if(sc.isFailure()) {
    msg(MSG::FATAL) << " Could not get TrigTRT_DCCollByteStreamTool tool"<<endreq;
    return sc;
  }
  //  Get ROBDataProvider
  sc = m_robDataProvider.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::FATAL) << " Can't get ROBDataProviderSvc " << endreq;
    return sc;
  }
  sc = m_trtByteStreamErrorService.retrieve();
  if (sc.isFailure())
   {
     msg(MSG::FATAL) << "Failed to retrieve service " << m_trtByteStreamErrorService << endreq;
     return sc;
   } 

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    msg(MSG::INFO) << "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("DCTool.RegSel");
    m_timer[1] = timerSvc->addItem("DCToolRobProv");
    m_timer[2] = timerSvc->addItem("DCTool.TRTConv");
    m_timer[3] = timerSvc->addItem("DCTool.Total");
  }
  
  return sc;
}

//-------------------------------------------------------------------------

StatusCode TrigTRT_DriftCircleProviderTool::fillCollections(const IRoiDescriptor &roi)
{ //std::cout<<" in TrigTRT_DriftCircleProviderTool::convertBStoDCColl "<<std::endl;;

  m_time_regsel  = 0.0;
  m_time_robprov = 0.0;
  m_time_trtcnv  = 0.0;
  m_time_total   = 0.0;

  if(m_timers)
    {
      m_timer[3]->start();
    }
  

//   Get TRT Drift Circles 
//
  std::vector<unsigned int> uIntListOfTRTRobs;
  
  if(m_timers) m_timer[0]->start();
  // m_regionSelector->DetROBIDListUint( TRT, m_zMin, m_zMax, roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax, uIntListOfTRTRobs);
  m_regionSelector->DetROBIDListUint( TRT, roi, uIntListOfTRTRobs);
  if(m_timers) m_timer[0]->pause();
  if(m_timers) m_timer[1]->start();
  
  if (m_uIntListOfTRT_Robs.size() > 0) {

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG)<< " Pre-loading the  following TRT RoBs: " << endreq;
      for ( std::vector<unsigned int>::iterator iTrtRob=m_uIntListOfTRT_Robs.begin() ; 
        iTrtRob!=m_uIntListOfTRT_Robs.end() ;  iTrtRob++) {
        msg(MSG::DEBUG) << *iTrtRob << " == 0x" << std::hex << *iTrtRob << std::dec << endreq;
        }
      }
      m_robDataProvider->addROBData( m_uIntListOfTRT_Robs );
   } 
  else 
    m_robDataProvider->addROBData( uIntListOfTRTRobs );
  
  if(m_timers) m_timer[1]->pause();
  if(m_timers) m_timer[0]->resume();
  std::vector<IdentifierHash> listOfTRTIds; 
  //m_regionSelector->DetHashIDList( TRT, m_zMin, m_zMax, roiEtaMin, roiEtaMax, roiPhiMin, roiPhiMax,&listOfTRTIds);
  m_regionSelector->DetHashIDList( TRT, roi , listOfTRTIds);
  if(m_timers) m_timer[0]->pause();
  

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "REGTEST / TRT : Roi contains " 
					   << listOfTRTIds.size() << " det. Elements" << endreq;

  std::vector<const ROBF*> v;
      
  if(m_timers) m_timer[1]->resume();
  if (m_uIntListOfTRT_Robs.size() > 0) 
    m_robDataProvider->getROBData(m_uIntListOfTRT_Robs,v);
  else	
    m_robDataProvider->getROBData(uIntListOfTRTRobs,v);
      
  std::vector<ROBData> vRobData;
  IROBDataProviderSvc::VROBFRAG::const_iterator it = v.begin(); 
  IROBDataProviderSvc::VROBFRAG::const_iterator it_e = v.end(); 
  for(; it!=it_e;++it)
    { 
      vRobData.push_back(ROBData((*it))); 
    }    
  
  if(m_timers) 
    {
      m_timer[1]->pause();
      m_timer[2]->start();
    }

  m_trtDataErrors.clear();
  m_trtByteStreamErrorService->resetCounts(); 

  StatusCode sc = m_TRT_DCCollBSTool->convert(vRobData,listOfTRTIds);

  TRTByteStreamErrors::errorTypes Trt_Bs_Errors[6] = 
    {TRTByteStreamErrors::BCIDError, 
     TRTByteStreamErrors::L1IDError, 
     TRTByteStreamErrors::MISSINGError,
     TRTByteStreamErrors::ERRORError,
     TRTByteStreamErrors::SIDError,
     TRTByteStreamErrors::RobStatusError};  

  if(sc.isRecoverable())
    {
      for(int idx=0;idx<MAXNUM_TRT_BS_ERRORS_3;idx++)
	{
	  int n_errors = m_trtByteStreamErrorService->getNumberOfErrors(Trt_Bs_Errors[idx]);
	  for (int ierror = 0; ierror<n_errors;ierror++)
	    m_trtDataErrors.push_back(idx);
	}
    }

  if(m_timers) 
    {
      m_timer[3]->stop();
      m_timer[1]->stop();
      m_timer[2]->stop();
      m_timer[0]->stop();
      m_time_regsel  = m_timer[0]->elapsed();
      m_time_robprov = m_timer[1]->elapsed();
      m_time_trtcnv  = m_timer[2]->elapsed();
      m_time_total   = m_timer[3]->elapsed();
    }
  
  return sc;
}
//-------------------------------------------------------------------------

StatusCode TrigTRT_DriftCircleProviderTool::fillCollections(std::vector<IdentifierHash> listOfTRTIds,std::vector<const ROBF*> v)
{ //std::cout<<" in TrigTRT_DriftCircleProviderTool::convertBStoDCColl "<<std::endl;;

  m_time_regsel  = 0.0;
  m_time_robprov = 0.0;
  m_time_trtcnv  = 0.0;
  m_time_total   = 0.0;

  if(m_timers)
    {
      m_timer[3]->start();
    }
  

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "REGTEST / TRT : Roi contains " 
					   << listOfTRTIds.size() << " det. Elements" << endreq;
#endif  
      
  if(m_timers) m_timer[1]->resume();
  //      m_robDataProvider->getROBData(uIntListOfTRTRobs,v);
      
  std::vector<ROBData> vRobData;
  IROBDataProviderSvc::VROBFRAG::const_iterator it = v.begin(); 
  IROBDataProviderSvc::VROBFRAG::const_iterator it_e = v.end(); 
  for(; it!=it_e;++it)
    { 
      vRobData.push_back(ROBData((*it))); 
    }    
	  
  if(m_timers) 
    {
      m_timer[1]->pause();
      m_timer[2]->start();
    }

  m_trtDataErrors.clear(); 
  m_trtByteStreamErrorService->resetCounts();
  StatusCode sc = m_TRT_DCCollBSTool->convert(vRobData,listOfTRTIds);

  TRTByteStreamErrors::errorTypes Trt_Bs_Errors[6] = 
    {TRTByteStreamErrors::BCIDError, 
     TRTByteStreamErrors::L1IDError, 
     TRTByteStreamErrors::MISSINGError,
     TRTByteStreamErrors::ERRORError,
     TRTByteStreamErrors::SIDError,
     TRTByteStreamErrors::RobStatusError};

  if(sc.isRecoverable())
    {
      for(int idx=0;idx<MAXNUM_TRT_BS_ERRORS_3;idx++)
	{
	  int n_errors = m_trtByteStreamErrorService->getNumberOfErrors(Trt_Bs_Errors[idx]);
	  for (int ierror = 0; ierror<n_errors;ierror++)
	    m_trtDataErrors.push_back(idx);
	}
    }

  if(m_timers) 
    {
      m_timer[3]->stop();
      m_timer[1]->stop();
      m_timer[2]->stop();
      m_timer[0]->stop();
      m_time_regsel  = m_timer[0]->elapsed();
      m_time_robprov = m_timer[1]->elapsed();
      m_time_trtcnv  = m_timer[2]->elapsed();
      m_time_total   = m_timer[3]->elapsed();
    }
  
  return sc;
}

std::string TrigTRT_DriftCircleProviderTool::trtContainerName()
{ 
  return m_TRT_DCCollBSTool->trtContainerName(); 
}

const std::vector<int>* TrigTRT_DriftCircleProviderTool::fillTRT_DataErrors()
{
  return &m_trtDataErrors;
}
