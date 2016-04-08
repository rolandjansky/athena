/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBAccessTest.h
 *
 * @brief Definition of RDBAccessTest class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBAccessTest.h,v 1.3 2005-01-14 10:43:31 tsulaia Exp $
 */
#ifndef RDBACCESSTEST_H
#define RDBACCESSTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"

class IRDBRecordset;

/**
 * @class RDBAccessTest
 *
 * @brief RDBAccessTest is an example algorithm demonstrating the usage of RDBAccessSvc
 * 
 */

class RDBAccessTest:public AthAlgorithm 
{
 public:
  /// Standard constructor
  RDBAccessTest(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  ~RDBAccessTest();
  
  /// Connect to the database, get recordset with TIFG data corresponding
  /// to ATLAS-00, then disconnect from database
  virtual StatusCode initialize();

  /// Navigate the recordset by index and by iterator
  /// Print out the contents 
  virtual StatusCode execute();

  /// Do nothing
  virtual StatusCode finalize();

private:
  const IRDBRecordset* m_tifg;
};

#endif
