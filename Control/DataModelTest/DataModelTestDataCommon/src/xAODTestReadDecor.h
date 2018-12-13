// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/src/xAODTestReadDecor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Algorithm to test reading decorations using handles.
 */


#ifndef DATAMODELTESTDATACOMMON_XAODTESTREADDECOR_H
#define DATAMODELTESTDATACOMMON_XAODTESTREADDECOR_H


#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadDecorHandleKey.h"


namespace DMTest {


/**
 * @brief Algorithm to test reading decorations using handles.
 */
class xAODTestReadDecor
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param svc The service locator.
   */
  xAODTestReadDecor (const std::string &name, ISvcLocator *pSvcLocator);
  

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

  /// Parameter: Root names of the objects.
  std::string m_cvecName;
  std::string m_cinfoName;

  /// Handles for reading decorations.
  SG::ReadDecorHandleKey<CVec> m_cvecDecorKey;
  SG::ReadDecorHandleKey<C>    m_cinfoDecorKey;
  SG::ReadDecorHandleKey<C>    m_cinfoBaseDecorKey;
  SG::ReadDecorHandleKey<SG::AuxElement> m_objDecorKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_XAODTESTREADDECOR_H
