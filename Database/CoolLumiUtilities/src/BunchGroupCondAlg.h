// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/BunchGroupCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Conditions algorithm to unpack bunch group data from COOL.
 */


#ifndef COOLLUMIUTILITIES_BUNCHGROUPCONDALG_H
#define COOLLUMIUTILITIES_BUNCHGROUPCONDALG_H


#include "CoolLumiUtilities/BunchGroupCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Conditions algorithm to unpack bunch group data from COOL.
 */
class BunchGroupCondAlg
  : public AthReentrantAlgorithm
{
public:
  /// Forward base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /// Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Algorithm execute method.
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  /// Input conditions object.
  SG::ReadCondHandleKey<AthenaAttributeList> m_bunchGroupFolderInputKey
  { this, "BunchGroupFolderInputKey", "", "" };

  /// Output conditions object.
  SG::WriteCondHandleKey<BunchGroupCondData> m_bunchGroupOutputKey
  {this, "BunchGroupOutputKey", "BunchGroupCondData", "" };
};


#endif // not COOLLUMIUTILITIES_BUNCHGROUPCONDALG_H
