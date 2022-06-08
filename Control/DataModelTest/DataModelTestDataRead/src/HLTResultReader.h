// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DataModelTestDataRead/src/HLTResultReader.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Test for serializing an xAOD object from bytestream.
 */


#ifndef DATAMODELTESTDATAREAD_HLTRESULTREADER_H
#define DATAMODELTESTDATAREAD_HLTRESULTREADER_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "CxxUtils/checker_macros.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigNavigation/Navigation.h"


namespace HLT {
  class HLTResult;
}


namespace DMTest {


/**
 * @brief Test for serializing an xAOD object from bytestream.
 */
class HLTResultReader
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  HLTResultReader (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute() override;


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize() override;


private:
  /// Handle to read the HLTResult object.
  SG::ReadHandleKey<HLT::HLTResult> m_resultKey;

  /// Navigation object use to fill the result.
  ToolHandle<HLT::Navigation>        m_nav;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_HLTRESULTREADER_H
