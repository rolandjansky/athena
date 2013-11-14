/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MuonboyAth/MboyDigiEmptyLoop.h"

#include "MboyEDMToolInterfaces/IMboyGetPrepDataFromEventTool.h"

MboyDigiEmptyLoop::MboyDigiEmptyLoop(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 p_IMboyGetPrepDataFromEventTool ( "MboyGetPrepDataFromEventTool" ) 
{

//Declare the properties

  declareProperty("SwitchOff" , m_SwitchOff = 0 ) ;

  declareProperty("MboyGetPrepDataFromEventTool" , p_IMboyGetPrepDataFromEventTool ) ;

}
 
// Initialize
StatusCode MboyDigiEmptyLoop::initialize(){


  ATH_MSG_INFO( "Initialisation started	  " ) ;

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are        " ) ;
  if (m_SwitchOff == 0) {
    ATH_MSG_INFO( "= SwitchOff                          " << m_SwitchOff          ) ;
    ATH_MSG_INFO( "================================" ) ;
  }
  ATH_MSG_INFO( "= SwitchOff                 " << m_SwitchOff                   ) ;
  ATH_MSG_INFO( "================================" ) ;


//Retrieve p_IMboyGetPrepDataFromEventTool
  if ( p_IMboyGetPrepDataFromEventTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMboyGetPrepDataFromEventTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyGetPrepDataFromEventTool ) ;
  

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}
 
// Execute
StatusCode MboyDigiEmptyLoop::execute(){

  if (m_SwitchOff == 1) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;

//Collect PrepRawData
  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  VectorOfpPrepData.clear();

  sc=p_IMboyGetPrepDataFromEventTool->CollectPrepData(VectorOfpPrepData);
  if ( sc.isFailure() ) ATH_MSG_FATAL(  " CollectPrepData failed" ) ;
  if ( sc.isFailure() ) return( StatusCode::FAILURE );


  return StatusCode::SUCCESS;

}
 
// Finalize
StatusCode MboyDigiEmptyLoop::finalize(){return StatusCode::SUCCESS;}
