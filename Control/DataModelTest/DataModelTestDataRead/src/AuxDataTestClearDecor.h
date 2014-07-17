// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/AuxDataTestClearDecor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test clearing @c DataVector decorations.
 */


#ifndef DATAMODELTESTDATAREAD_AUXDATATESTCLEARDECOR_H
#define DATAMODELTESTDATAREAD_AUXDATATESTCLEARDECOR_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm to test adding decorations to a @c DataVector
 *        with auxiliary data.
 */
class AuxDataTestClearDecor
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  AuxDataTestClearDecor (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  /// Parameter: Prefix for names read from SG.
  std::string m_readPrefix;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_AUXDATATESTCLEARDECOR_H
