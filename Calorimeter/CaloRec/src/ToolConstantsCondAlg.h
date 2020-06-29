// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/ToolConstantsCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2020
 * @brief Convert from COOL inline data / POOL data to ToolConstants.
 */


#ifndef CALOREC_TOOLCONSTANTSCONDALG_H
#define CALOREC_TOOLCONSTANTSCONDALG_H


#include "CaloRec/Blob2ToolConstants.h"
#include "CaloConditions/ToolConstants.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"


/**
 * @brief Convert from COOL inline data to ToolConstants.
 *
 * For some tools, the input data are saved as COOL online data rather
 * than as a ToolConstants object directly.  This conditions algorithm
 * will convert these data to a ToolConstants object in the condition store.
 *
 * In some cases, we may also want to read a POOL file directly.
 * In this case, the ToolConstants objects are available in the detector store.
 * To handle this, we can also copy these objects to the conditions store.
 *
 * To read from COOL, the COOLFolderKey property should be set.
 * To read from the detector store, the DetStoreKey property should be set.
 */
class ToolConstantsCondAlg
  : public AthReentrantAlgorithm
{
public:
  /// Inherit constructor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /// Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Gaudi execute method.
  StatusCode execute (const EventContext& ctx) const override;


private:
  ToolHandle<Blob2ToolConstants> m_blobTool
  { this, "BlobTool", "Blob2ToolConstants", "Tool to convert from COOL inline ata." };

  SG::ReadCondHandleKey<CondAttrListCollection> m_coolFolderKey
  { this, "COOLFolderKey", "", "Name of COOL folder" };

  StringProperty m_detStoreKey
  { this, "DetStoreKey", "", "Key in DetetorStore of ToolConstants object." };

  SG::WriteCondHandleKey<CaloRec::ToolConstants> m_toolConstantsKey
  { this, "ToolConstantsKey", "", "SG key of output ToolConstants object" };
};


#endif // not CALOREC_TOOLCONSTANTSCONDALG_H
