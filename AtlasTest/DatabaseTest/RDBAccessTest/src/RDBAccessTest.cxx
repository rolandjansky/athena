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
#include "GaudiKernel/ServiceHandle.h"

RDBAccessTest::RDBAccessTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{ 
}

RDBAccessTest::~RDBAccessTest()
{ 
}

StatusCode RDBAccessTest::initialize()
{
  ATH_MSG_INFO( " initializing " );

  ServiceHandle<IRDBAccessSvc> iAccessSvc ("RDBAccessSvc", name());
  ATH_CHECK( iAccessSvc.retrieve() );

  iAccessSvc->connect();
  ATH_MSG_INFO( " Tag for TIFG is " << iAccessSvc->getChildTag("TIFG","ATLAS-00","ATLAS",false) );

  m_tifg = iAccessSvc->getRecordset("TIFG","ATLAS-00","ATLAS");
  iAccessSvc->disconnect();

  ATH_MSG_INFO( " TIFG data corresponding to ATLAS-00 fetched " );

  return StatusCode::SUCCESS;
}

StatusCode RDBAccessTest::execute() 
{
  unsigned int ind;
  ATH_MSG_INFO( " executing " );

  ATH_MSG_INFO( " --> Number of records fetched = " << m_tifg->size() );

  ATH_MSG_INFO( " Iterating by index " );
  for(ind = 0; ind < m_tifg->size(); ind++)
  {
    const IRDBRecord* rec = (*m_tifg)[ind];
    ATH_MSG_INFO( "   STRUCTURE " << ind );
    ATH_MSG_INFO( "     section = " << rec->getInt("SECTION") );
    ATH_MSG_INFO( "     nelem   = " << rec->getInt("NELEM") );
    ATH_MSG_INFO( "     dz      = " << rec->getDouble("DZ") );
  }

  ATH_MSG_INFO( " ****** \n\n " );

  ATH_MSG_INFO( " Using iterator " );
  IRDBRecordset::const_iterator first = m_tifg->begin();
  IRDBRecordset::const_iterator last = m_tifg->end();

  ind = 0;
  for(; first!=last; first++)
  {
    ATH_MSG_INFO( "   STRUCTURE " << ind++ );
    ATH_MSG_INFO( "     section = " << (*first)->getInt("SECTION") );
    ATH_MSG_INFO( "     nelem   = " << (*first)->getInt("NELEM") );
    ATH_MSG_INFO( "     dz      = " << (*first)->getDouble("DZ") );
  }

  return StatusCode::SUCCESS;
}

StatusCode RDBAccessTest::finalize()
{
  ATH_MSG_INFO(" finalizing " );
  return StatusCode::SUCCESS;
}

