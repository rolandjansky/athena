// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/MetaWriterAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2017
 * @brief Write some metadata objects for testing purposes.
 */


#ifndef DATAMODELTESTDATACOMMON_METAWRITERALG_H
#define DATAMODELTESTDATACOMMON_METAWRITERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"


namespace DMTest {


class MetaWriterAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name The algorithm name.
   * @param pSvcLocator The service locator.
   */
  MetaWriterAlg (const std::string &name, ISvcLocator *pSvcLocator);


  /**
   * @brief Algorithm initialization; called at the beginning of the job.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Algorithm execute method.
   */
  virtual StatusCode execute() override;


private:
  ServiceHandle<StoreGateSvc> m_metaStore;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_METAWRITERALG_H
