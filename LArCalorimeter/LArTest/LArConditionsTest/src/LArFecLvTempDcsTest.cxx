/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArFecLvTempDcsTest.cxx
 *
 * @brief This file contains an algorithm for testing lar conditions
 * data access
 *
 * @author Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 *
 * $Id: LArFecLvTempDcsTest.cxx,v 1.2 2009-01-15 15:06:51 gunal Exp $ */

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgTool.h"

#include "LArConditionsTest/LArFecLvTempDcsTest.h" 
#include "LArRawConditions/LArFecLvTempDcs.h" 
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArFecLvTempDcsTest::LArFecLvTempDcsTest(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name,pSvcLocator),
        m_tool("LArFecLvTempDcsTool")
{
  declareProperty("LArFecLvTempDcsTool",m_tool,"tool for LArFecLvTempDcs");
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArFecLvTempDcsTest::~LArFecLvTempDcsTest()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

StatusCode LArFecLvTempDcsTest::initialize()
{
    MsgStream  log(messageService(),name());
    StatusCode sc;
    sc = m_tool.retrieve();
    if ( sc.isFailure() ){
	log << MSG::ERROR << "Could not fetch LArFecLvTempDcsTool" << endreq;
	return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

StatusCode LArFecLvTempDcsTest::execute()
{
    MsgStream  log(messageService(),name());  
    log<< MSG::DEBUG<<" LArCondDataTest::execute() "<<endreq;

    StoreGateSvc* detStore;
    StatusCode sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
      return sc;
    }
    
    const LArOnlineID* onlineID;
    sc = detStore->retrieve(onlineID, "LArOnlineID");
    if (sc.isFailure()) {
      return sc;
    }

    float test = 0;
    LArFecLvTempDcs fec;
    sc = m_tool->getV1("crate_H13",test);
    sc = m_tool->getFec("crate_H13",fec);
    log << MSG::DEBUG << "Voltage for crate_H13 " << test << endreq;
    log << MSG::DEBUG << "Data from crate_H13 " << endreq;
    fec.print( log );
    test = 0;
    sc = m_tool->getV1("atllarclvtemp:crate_H13",test);
    log << MSG::DEBUG << "Voltage for atllarclvtemp:crate_H13 " << test << endreq;
    test = 0;
    sc = m_tool->getV1("abccrate_H13def",test);
    log << MSG::DEBUG << "Voltage for abccrate_H13def " << test << endreq;

    for ( std::vector<HWIdentifier>::const_iterator i=onlineID->feb_begin();
	  i!=onlineID->feb_end();++i) {
        log << MSG::DEBUG << " HWIdentifier : " << *i << endreq;
	m_tool->getFec(*i,fec);
	fec.print( log );
    }

    return sc ;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
// Note that it is NOT NECESSARY to run the finalize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode LArFecLvTempDcsTest::finalize()
{
  MsgStream  log(messageService(),name());

  log<<MSG::DEBUG << " finalize "<<endreq;

  return StatusCode::SUCCESS; 

} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
