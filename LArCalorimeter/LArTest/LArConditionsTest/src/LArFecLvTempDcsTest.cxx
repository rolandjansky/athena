/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "LArIdentifier/LArOnlineID.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArFecLvTempDcsTest::LArFecLvTempDcsTest(const std::string& name, ISvcLocator* pSvcLocator) :
	AthAlgorithm(name,pSvcLocator),
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
  ATH_CHECK( m_tool.retrieve() );
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

StatusCode LArFecLvTempDcsTest::execute()
{
    ATH_MSG_DEBUG(" LArCondDataTest::execute() ");

    const LArOnlineID* onlineID = nullptr;
    ATH_CHECK( detStore()->retrieve(onlineID, "LArOnlineID") );

    float test = 0;
    LArFecLvTempDcs fec;
    StatusCode sc = m_tool->getV1("crate_H13",test);
    sc = m_tool->getFec("crate_H13",fec);
    ATH_MSG_DEBUG ( "Voltage for crate_H13 " << test );
    ATH_MSG_DEBUG ( "Data from crate_H13 " );
    fec.print( msg() );
    test = 0;
    sc = m_tool->getV1("atllarclvtemp:crate_H13",test);
    ATH_MSG_DEBUG ( "Voltage for atllarclvtemp:crate_H13 " << test );
    test = 0;
    sc = m_tool->getV1("abccrate_H13def",test);
    ATH_MSG_DEBUG ( "Voltage for abccrate_H13def " << test );

    for ( std::vector<HWIdentifier>::const_iterator i=onlineID->feb_begin();
	  i!=onlineID->feb_end();++i) {
        ATH_MSG_DEBUG ( " HWIdentifier : " << *i );
	m_tool->getFec(*i,fec);
	fec.print( msg() );
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
  ATH_MSG_DEBUG ( " finalize ");
  return StatusCode::SUCCESS; 
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
