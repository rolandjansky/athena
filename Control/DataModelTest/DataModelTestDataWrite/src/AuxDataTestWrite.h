// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/AuxDataTestWrite.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test writing @c DataVector with auxiliary data.
 */


#ifndef DATAMODELTESTDATAWRITE_AUXDATATESTWRITE_H
#define DATAMODELTESTDATAWRITE_AUXDATATESTWRITE_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm for creating test aux data.
 */
class AuxDataTestWrite
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  AuxDataTestWrite (const std::string &name, ISvcLocator *pSvcLocator);
  

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


private:
  /// Event counter.
  int m_count;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_AUXDATATESTWRITE_H
