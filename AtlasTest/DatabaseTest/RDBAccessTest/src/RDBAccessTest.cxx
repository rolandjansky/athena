/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBAccessTest.cxx
 *
 * @brief Implementation of RDBAccessTest class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBAccessTest.cxx,v 1.8 2007-01-17 13:29:43 tsulaia Exp $
 */

#include "RDBAccessTest/RDBAccessTest.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/MsgStream.h"

RDBAccessTest::RDBAccessTest(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_locator(pSvcLocator)
{ 
}

RDBAccessTest::~RDBAccessTest()
{ 
}

StatusCode RDBAccessTest::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " initializing " << endreq;

  IRDBAccessSvc* iAccessSvc;
  StatusCode sc = m_locator->service("RDBAccessSvc",iAccessSvc);

  if(sc.isFailure())
    return sc;

  iAccessSvc->connect();
  log << MSG::INFO << " Tag for TIFG is " << iAccessSvc->getChildTag("TIFG","ATLAS-00","ATLAS",false) << endreq;

  m_tifg = iAccessSvc->getRecordset("TIFG","ATLAS-00","ATLAS");
  iAccessSvc->disconnect();

  log << MSG::INFO << " TIFG data corresponding to ATLAS-00 fetched " << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RDBAccessTest::execute() 
{
  unsigned int ind;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " executing " << endreq;

  log << MSG::INFO << " --> Number of records fetched = " << m_tifg->size() << endreq;

  log << MSG::INFO << " Iterating by index " << endreq;
  for(ind = 0; ind < m_tifg->size(); ind++)
  {
    const IRDBRecord* rec = (*m_tifg)[ind];
    log << MSG::INFO << "   STRUCTURE " << ind << endreq;
    log << MSG::INFO << "     section = " << rec->getInt("SECTION") << endreq;
    log << MSG::INFO << "     nelem   = " << rec->getInt("NELEM") << endreq;
    log << MSG::INFO << "     dz      = " << rec->getDouble("DZ") << endreq;
  }

  log << MSG::INFO << " ****** \n\n " << endreq;

  log << MSG::INFO << " Using iterator " << endreq;
  IRDBRecordset::const_iterator first = m_tifg->begin();
  IRDBRecordset::const_iterator last = m_tifg->end();

  ind = 0;
  for(; first!=last; first++)
  {
    log << MSG::INFO << "   STRUCTURE " << ind++ << endreq;
    log << MSG::INFO << "     section = " << (*first)->getInt("SECTION") << endreq;
    log << MSG::INFO << "     nelem   = " << (*first)->getInt("NELEM") << endreq;
    log << MSG::INFO << "     dz      = " << (*first)->getDouble("DZ") << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RDBAccessTest::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO <<" finalizing " << endreq;

  return StatusCode::SUCCESS;
}

