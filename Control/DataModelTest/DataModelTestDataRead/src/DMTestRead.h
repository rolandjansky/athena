// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file  DataModelTestDataRead/DMTestRead.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Algorithm to test reading @c DataVector data.
 *
 *        We read information using all four types,
 *        @c BVec, @c DVec, @c BDer, @c DDer.
 *        When we run, we have the inheritance relationship set up
 *        between @c BVec and @c DVec.
 */

#ifndef DATAMODELTESTDATAREAD_DMTESTREAD_H
#define DATAMODELTESTDATAREAD_DMTESTREAD_H


#include "AthenaBaseComps/AthAlgorithm.h"



namespace DMTest {


/**
 * @brief Algorithm for printing test data.
 */
class DMTestRead
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  DMTestRead (const std::string &name,ISvcLocator *pSvcLocator);

  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute() override;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_DMTESTREAD_H
