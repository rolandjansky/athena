/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringTest/dummyAlgoHLTErrorCode.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Sleep.h"
#include "AthenaKernel/Timeout.h"


using namespace PESA;


dummyAlgoHLTErrorCode::dummyAlgoHLTErrorCode(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator), m_error(HLT::ERROR)
{
  declareProperty("HLTErrorCode", m_paramSet = "ERROR", "This dummy algorithm will return the given HLT::ErrorCode");
  declareProperty("ThrowStdException", m_stdException = false, "this dummy algorithm will throw a std execption");
  declareProperty("ThrowGaudiException", m_gaudiException = false, "this dummy algorithm will throw a Gaudi execption");
  declareProperty("ThrowUnknownException", m_unknownException = false, "this dummy algorithm will throw a Unkown execption");
  declareProperty("BurnTimeAndCheckTimeout" , m_burnTime = -1, "Burn time (in nanoseconds). In addition check timeout every millisecond");
}

HLT::ErrorCode dummyAlgoHLTErrorCode::hltInitialize()
{
  // set eror from string
  int pos = HLT::getErrorCodePosFromStr( m_paramSet );
  if ( pos  > -1 )
    m_error = static_cast<HLT::ErrorCode>(pos);
  else
    msg() << MSG::WARNING << "could not find HLT::ErrorCode:  " <<  m_paramSet
	  << " .. will use default value = ERROR, please check your jobProperty!" << endreq;

  return HLT::OK;
}


HLT::ErrorCode dummyAlgoHLTErrorCode::hltExecute(const HLT::TriggerElement* te_in,
				     HLT::TriggerElement* te_out)
{
  msg() << MSG::INFO << "Executing this dummyAlgoHLTErrorCode " << name() << " for types "
	<< te_in->getId() << " -> " << te_out->getId() << endreq;

  if (m_burnTime>0) {
    long nSleep = static_cast<long>(m_burnTime/1e6);   // number of 1 ms sleeps
    for (long i=0; i<nSleep; ++i) {
      if ( Athena::Timeout::instance().reached() ) {
        return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
      }
      Gaudi::NanoSleep(static_cast<long>(1e6));   // sleep 1 ms
    }
  }
  
  if (m_unknownException) {
    msg() << MSG::INFO << " through an unknown exception ... " << endreq;
    throw PESA::dummyException();

  }
  if (m_gaudiException) {
    msg() << MSG::INFO << " through a Gaudi exception ... " << endreq;
    throw GaudiException("this is a dummy GaudiException", "dummy Tag", StatusCode::FAILURE);
  }
  if (m_stdException) {
    msg() << MSG::INFO << " through an std exception ... " << endreq;
    std::vector<int> a;
    a.at(2) = 5;
  }


  msg() << MSG::INFO << "return value = " << HLT::strErrorCode(m_error) << endreq;
  return m_error;
}
