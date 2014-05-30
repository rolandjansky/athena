/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: LArFecLvTempDcsTool.cxx,v 1.3 2008-10-17 13:00:09 gunal Exp $
//
// Description: Tool to provide FEC LV TEMP DCS information.
//
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Denis Oliveira Damazio <damazio@bnl.gov>
//
//-----------------------------------------------------------------------

#include "LArIdentifier/LArIdManager.h"
#include "LArCondUtils/LArFecLvTempDcsTool.h" 
#include "LArRawConditions/LArFecLvTempDcs.h" 

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArIdentifier/LArOnlineID.h"
#include <iostream> 
#include <string> 

// constructor 
LArFecLvTempDcsTool::LArFecLvTempDcsTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : AlgTool(type,name,parent), m_foldername("/LAR/DCS/CLVTEMP")
{
 declareInterface< ILArFecLvTempDcsTool >( this );
 declareProperty("FolderName",m_foldername);

}

// destructor 
LArFecLvTempDcsTool::~LArFecLvTempDcsTool()
{ }

// intialize 
StatusCode LArFecLvTempDcsTool::initialize()
{
  MsgStream  log(msgSvc(),name());

  StatusCode sc = service("StoreGateSvc",StoreGate); 
  if(sc!=StatusCode::SUCCESS) return sc; 
  else log << MSG::DEBUG << "Retrieved StoreGateSvc" << endreq;
  
  sc = service("DetectorStore",m_detStore); 
  if(sc!=StatusCode::SUCCESS) return sc; 
  else log << MSG::DEBUG << "Retrieved Detector Store" << endreq;

  const LArIdManager* larMgr;
  sc= m_detStore->retrieve(larMgr);
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve LArIdManager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
  } else {
      log << MSG::DEBUG << "Successfully retrieved LArIdManager from DetectorStore" << endreq;
  } 
  // retrieve LArOnlineID
  m_larOnlineId = larMgr->getOnlineID();
  if (!m_larOnlineId) {
      log << MSG::ERROR
          << "Unable to retrieve pointer to LArOnlineID  "
          << endreq;
      return sc;
  }
  log << MSG::DEBUG << "Retrieved LArOnlineID" << endreq;


  sc= m_detStore->regHandle(m_atrlistcol,m_foldername); 
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to register DataHandle "<<m_foldername<<endreq;
      return StatusCode::FAILURE;
  } else {
      log << MSG::DEBUG << "Successfully registered DataHandle<CondAttrListCollection>" << endreq;
  } 
  
  return StatusCode::SUCCESS;
}




StatusCode LArFecLvTempDcsTool::getV1( const std::string& cratename, 
			float&  hv  ) 
{
  MsgStream  log(msgSvc(),name());

/*
  // first print event number and time details
  const EventInfo* event;
  if (StatusCode::SUCCESS==StoreGate->retrieve(event)) {
    int time=event->event_ID()->time_stamp();
    log << MSG::INFO << "In run/event [" << event->event_ID()->run_number() <<
      "," << event->event_ID()->event_number() << "] timestamp " << time <<
      endreq;
  } else {
    log << MSG::ERROR << "Could not get pointer to event" << endreq;
  }


*/

   bool crate_found = false;
   if ( m_atrlistcol.isValid() ) 
   {
      // loop over collection
      for (CondAttrListCollection::const_iterator citr=m_atrlistcol->begin();
           citr!=m_atrlistcol->end();++citr) {
        log << MSG::DEBUG << (((*citr).second)["DPNAME"]).data<std::string>()
		<< endreq;
	if ( ((((*citr).second)["DPNAME"]).data<std::string>()).find(cratename) 
			!= std::string::npos ){
		crate_found = true;
       		hv = (((*citr).second)["V1_VOLT"]).data<float>();
		// Dont search anymore
		break;
	}
      }
    } else {
      log << MSG::INFO << " No valid CondAttrListCollection with key " <<
        m_foldername << endreq;
    }
    if ( !crate_found ){
	log << MSG::ERROR << "Crate : " << cratename << " Not Found" << endreq;
	return StatusCode::SUCCESS; 
    }

    return StatusCode::SUCCESS; 
} 

StatusCode LArFecLvTempDcsTool::getFec( const std::string& cratename, 
			LArFecLvTempDcs&  fec  ) 
{
  MsgStream  log(msgSvc(),name());

/*
  // first print event number and time details
  const EventInfo* event;
  if (StatusCode::SUCCESS==StoreGate->retrieve(event)) {
    int time=event->event_ID()->time_stamp();
    log << MSG::INFO << "In run/event [" << event->event_ID()->run_number() <<
      "," << event->event_ID()->event_number() << "] timestamp " << time <<
      endreq;
  } else {
    log << MSG::ERROR << "Could not get pointer to event" << endreq;
  }
  const CondAttrListCollection* atrlistcol;

*/

  bool crate_found = false;
  // through joboptions
  if (m_atrlistcol.isValid() ) {
      // loop over collection
      for (CondAttrListCollection::const_iterator citr=m_atrlistcol->begin();
           citr!=m_atrlistcol->end();++citr) {
        log << MSG::VERBOSE << "trying to find : " 
		<<(((*citr).second)["DPNAME"]).data<std::string>()
		<< endreq;
	if ( ((((*citr).second)["DPNAME"]).data<std::string>()).find(cratename) 
			!= std::string::npos ){
		crate_found = true;
       		translateFec( (((*citr).second)), fec);
		// Dont search anymore
		break;
	}
      }
    } else {
      log << MSG::INFO << " DataHandle of CondAttrListCollection  " <<
        m_foldername << endreq;
    }
    if ( !crate_found ){
	log << MSG::ERROR << "Crate : " << cratename << " Not Found" << endreq;
	return StatusCode::SUCCESS; 
    }

	return StatusCode::SUCCESS; 
} 

StatusCode LArFecLvTempDcsTool::getFec( const HWIdentifier& crateID, 
			LArFecLvTempDcs&  fec  ) 
{
  MsgStream  log(msgSvc(),name());
  std::string partial_cratename =
	m_larOnlineId->feedthrough_name( crateID );
  std::string cratename("crate_");
  cratename+=partial_cratename;
  cratename.resize(cratename.length()-1);
  return getFec(cratename,fec);
}

void
LArFecLvTempDcsTool::translateFec( const coral::AttributeList att, 
	LArFecLvTempDcs& fec ) const
{
	// Temporary variables
	std::vector<float> volts;
	std::vector<float> temps;
	// Dump data into temporary vars
	volts.push_back(att["V1_VOLT"].data<float>() );
	volts.push_back(att["V2_VOLT"].data<float>() );
	volts.push_back(att["V3_VOLT"].data<float>() );
	volts.push_back(att["V4_VOLT"].data<float>() );
	volts.push_back(att["V5_VOLT"].data<float>() );
	volts.push_back(att["V6_VOLT"].data<float>() );
	volts.push_back(att["V7_VOLT"].data<float>() );
	temps.push_back(att["T1_TEMP"].data<float>() );
	temps.push_back(att["T2_TEMP"].data<float>() );
	temps.push_back(att["T3_TEMP"].data<float>() );
	temps.push_back(att["T4_TEMP"].data<float>() );
	fec.getVolts().voltage( volts );
	fec.getManifold().temperature( temps );
	fec.getOCEM().powerOn         ( att["OCEM_POWER"].data<bool>() );
	fec.getOCEM().powerOut        ( att["OCEM_OUTPUT"].data<bool>() );
	fec.getOCEM().interlock       ( att["OCEM_INTERLOCK"].data<bool>() );
	fec.getOCEM().overCurrent     ( att["OCEM_OVERCURRENT"].data<bool>() );
	fec.getOCEM().overTemperature ( att["OCEM_OVERTEMPERATURE"].data<bool>() );
	fec.getOCEM().remote          ( att["OCEM_REMOTE"].data<bool>() );
	fec.getOCEM().voltage         ( att["OCEM_CORTENSION"].data<float>() );
	fec.getOCEM().current         ( att["OCEM_CORCURRENT"].data<float>() );
	fec.getOCEM().on              ( att["OCEM_OCEMON"].data<bool>() );
	fec.getOCEM().out             ( att["OCEM_OCEMOUTPUT"].data<bool>() );
	fec.getOCEM().state           ( att["OCEM_STATE"].data<int>() );
	fec.getLVPS().allVoltGood     ( att["AVG"].data<bool>() );
	fec.getLVPS().allModGood      ( att["AMG"].data<bool>() );
	fec.getLVPS().overTemperature ( att["PS_OT"].data<bool>() );
	fec.getLVPS().overTemperatureOverRide ( att["PS_OT"].data<bool>() );
	fec.getLVPS().temperature     ( att["PS_TEMP"].data<float>() );
	fec.ELMBStat ( att["ELMBSTAT"].data<std::string>() == "OPERATIONAL" );
	fec.crateName ( att["DPNAME"].data<std::string>() );

}

