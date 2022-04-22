// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestDecor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Algorithm to test adding decorations to xAOD types.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTDECOR_H
#define DATAMODELTESTDATACOMMON_XAODTESTDECOR_H


#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test adding decorations to xAOD data.
 */
class xAODTestDecor
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestDecor (const std::string &name, ISvcLocator *pSvcLocator);
  

  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm event processing.
   */
  virtual StatusCode execute (const EventContext& ctx) const override; 


  /**
   * @brief Algorithm finalization; called at the end of the job.
   */
  virtual StatusCode finalize() override;


private:
  /// Parameter: Prefix for names read from SG.
  std::string m_readPrefix;

  /// Parameter: Name of the decoration.
  std::string m_decorName;

  /// Parameter: Offset to be applied to decoration.
  int m_offset;

  /// Flags.
  bool m_doCVec;
  bool m_doCInfo;
  bool m_doCTrig;

  /// Handles for writing decorations.
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EvtInfo", "EventInfo", "EventInfo name"};
  SG::WriteDecorHandleKey<CVec> m_cvecDecorKey;
  SG::WriteDecorHandleKey<C>    m_cinfoDecorKey;
  SG::WriteDecorHandleKey<SG::AuxElement> m_objDecorKey;
  SG::WriteDecorHandleKey<CVec> m_ctrigDecorKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTDECOR_H
