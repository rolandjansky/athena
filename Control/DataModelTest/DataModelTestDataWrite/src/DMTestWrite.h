// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DMTestWrite.h,v 1.2 2007-01-31 03:06:38 ssnyder Exp $

/**
 * @file  DataModelTestDataWrite/src/DMTestWrite.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Algorithm to test writing @c DataVector data.
 *
 *        We write information using all four types,
 *        @c BVec, @c DVec, @c BDer, @c DDer.
 *        In the data we write, there's no inheritance relationship
 *        between @c BVec and @c DVec.
 *
 * This line'll work around a cmt depcy bug by making the quote count even.
 */

#ifndef DATAMODELTESTDATAWRITE_DMTESTWRITE_H
#define DATAMODELTESTDATAWRITE_DMTESTWRITE_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test data.
 */
class DMTestWrite
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  DMTestWrite (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize();


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute(); 


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize();
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_DMTESTWRITE_H
