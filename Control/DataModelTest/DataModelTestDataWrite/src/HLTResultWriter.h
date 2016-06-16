// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/HLTResultWriter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Test for serializing an xAOD object into bytestream.
 */


#ifndef DATAMODELTESTDATAWRITE_HLTRESULTWRITER_H
#define DATAMODELTESTDATAWRITE_HLTRESULTWRITER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/ToolHandle.h"


namespace HLT {
  class HLTResult;
  class Navigation;
}


namespace DMTest {


/**
 * @brief Test for serializing an xAOD object into bytestream.
 */
class HLTResultWriter
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  HLTResultWriter (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute_r (const EventContext& ctx) const override;


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize() override;


private:
  /// Handle to write the HLTResult object.
  SG::WriteHandleKey<HLT::HLTResult> m_resultKey;

  /// Navigation object use to fill the result.
  ToolHandle<HLT::Navigation>        m_nav;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITE_HLTRESULTWRITER_H
