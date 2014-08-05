/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBEventInfoStreamerTool.h"
#include "TBEvent/TBEventInfo.h"

#include <string>
#include <ios>
#include <algorithm>

TBEventInfoStreamerTool::TBEventInfoStreamerTool(const std::string& name,
						 const std::string& type,
					         const IInterface* parent)
  : TBEventStreamerTool(name,type,parent)
{
   declareInterface<TBEventStreamerTool>(this);
  m_acceptTypes.resize(1,"Physics");
  declareProperty("AcceptedTypes",m_acceptTypes);
}

TBEventInfoStreamerTool::~TBEventInfoStreamerTool()
{ }

////////////////
// Initialize //
////////////////

StatusCode TBEventInfoStreamerTool::initializeTool()
{
  // messages
  MsgStream report(msgSvc(),name());

  m_selected_events = 0;

  for ( unsigned int i=0; i<m_acceptTypes.size(); i++ )
    {
      if ( m_acceptTypes[i] == "Physics" )
	{
	  m_acceptCodes.push_back(1);
	}
      else if ( m_acceptTypes[i] == "Special" )
	{
	  m_acceptCodes.push_back(0);
	}
      else if ( m_acceptTypes[i] == "f/e_Calibration" )
	{
	  m_acceptCodes.push_back(2);
	}
      else if ( m_acceptTypes[i] == "Random" )
	{
	  m_acceptCodes.push_back(3);
	}
      else if ( m_acceptTypes[i] == "BPC_Calibration" )
	{
	  m_acceptCodes.push_back(4);
	}
      else
	{
	     
	  report << MSG::FATAL
		 << "Detector event type not existing "
		 << m_acceptTypes << endreq;
	  report << MSG::FATAL
		 << "Possible types are: Special, Physics, f/e_calibration, Random, BPC_Calibration"
		 << endreq;
	  return StatusCode::FAILURE;
	}
     
    }

  if (m_acceptCodes.size()==0) {
    report << MSG::ERROR
		 << "No detector event type specified! Please specify at least one in the JobOptions" << endreq;
	  report << MSG::ERROR
		 << "Possible types are: Special, Physics, f/e_Calibration, Random, BPC_Calibration"
		 << endreq;
  } else {
    for ( unsigned int i=0; i<m_acceptCodes.size(); i++ )
      {
	report << MSG::DEBUG
	       << "Detector event code  found "
	       << m_acceptCodes[i] << endreq;
      }
  }

  return StatusCode::SUCCESS;
}

///////////////////
// Accept/Reject //
///////////////////

StatusCode TBEventInfoStreamerTool::accept()
{
  // messaging
  MsgStream report(msgSvc(),name());
 

  // retrieve Event Info
  const TBEventInfo* theEventInfo;
  StatusCode checkOut = m_storeGate->retrieve(theEventInfo,"TBEventInfo");
  if ( checkOut.isFailure() )
    {
      report << MSG::ERROR
	     << "cannot retrieve TBEventInfo from StoreGate"
	     << endreq;
      return StatusCode::FAILURE;
    }
  else
    {
      report << MSG::DEBUG
	     << "TBEventInfo object found in TDS"
	     << endreq;
    }
  int evtType = theEventInfo->getEventType();
  report << MSG::DEBUG << "Event Type found " << evtType << endreq;

  return (std::find(m_acceptCodes.begin(),m_acceptCodes.end(),evtType) != 
	  m_acceptCodes.end())  
    ? StatusCode::SUCCESS 
    : StatusCode::FAILURE;

}
