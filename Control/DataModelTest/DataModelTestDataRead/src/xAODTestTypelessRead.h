// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file src/xAODTestTypelessRead.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test reading xAOD objects with auxiliary data,
 *        without compile-time typing of aux data.
 */

#ifndef DATAMODELTESTDATAREAD_XAODTESTTYPELESSREAD_H
#define DATAMODELTESTDATAREAD_XAODTESTTYPELESSREAD_H


#include "AthenaBaseComps/AthAlgorithm.h"


namespace DMTest {


/**
 * @brief Algorithm for reading test xAOD data.
 */
class xAODTestTypelessRead
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestTypelessRead (const std::string &name, ISvcLocator *pSvcLocator);
  

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
  template <class OBJ, class AUX>
  StatusCode testit (const char* key);

  template <class OBJ>
  StatusCode testit_view (const char* key);

  /// Parameter: Prefix for names written to SG.  Null for no write.
  std::string m_writePrefix;

  /// Event counter.
  int m_count;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_XAODTESTTYPELESSREAD_H
