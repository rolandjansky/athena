// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


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


#include "AthenaBaseComps/AthReentrantAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test data.
 */
class DMTestWrite
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  DMTestWrite (const std::string &name, ISvcLocator *pSvcLocator);

  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute(const EventContext& ctx) const override;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_DMTESTWRITE_H
