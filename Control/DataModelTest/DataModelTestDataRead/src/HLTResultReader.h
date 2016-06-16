// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/src/HLTResultReader.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Test for serializing an xAOD object from bytestream.
 */


#ifndef DATAMODELTESTDATAREAD_HLTRESULTREADER_H
#define DATAMODELTESTDATAREAD_HLTRESULTREADER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"


namespace HLT {
  class HLTResult;
  class Navigation;
}


namespace DMTest {


/**
 * @brief Test for serializing an xAOD object from bytestream.
 */
class HLTResultReader
  : public AthReentrantAlgorithm
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
  /// Handle to read the HLTResult object.
  SG::ReadHandleKey<HLT::HLTResult> m_resultKey;

  /// Navigation object use to fill the result.
  ToolHandle<HLT::Navigation>        m_nav;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAREAD_HLTRESULTREADER_H
