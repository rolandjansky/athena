// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/MetaReaderAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2017
 * @brief Test reading metadata objects.
 */


#ifndef DATAMODELTESTDATACOMMON_METAREADERALG_H
#define DATAMODELTESTDATACOMMON_METAREADERALG_H


#include "DataModelTestDataCommon/S1Meta.h"
#include "DataModelTestDataCommon/CMeta.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadMetaHandleKey.h"
#include "StoreGate/ReadMetaHandle.h"


namespace DMTest {


class MetaReaderAlg
  : public AthReentrantAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param pSvcLocator The service locator.
   */
  MetaReaderAlg (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm execute method.
   * @param ctx Event context.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  SG::ReadMetaHandleKey<DMTest::S1> m_s1Key;
  SG::ReadMetaHandleKey<DMTest::C>  m_cKey;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_METAREADERALG_H
