// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/src/LBDurationCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Produce LBDurationCondData from /TRIGGER/LUMI/LBLB.
 */


#ifndef LUMIBLOCKCOMPS_LBDURATIONCONDALG_H
#define LUMIBLOCKCOMPS_LBDURATIONCONDALG_H


#include "LumiBlockData/LBDurationCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Produce LBDurationCondData from /TRIGGER/LUMI/LBLB.
 */
class LBDurationCondAlg
  : public AthReentrantAlgorithm
{
public:
  /// Forward base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /// Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Algorithm execute method.
  virtual StatusCode execute (const EventContext& ctx) const override;
  virtual bool isReEntrant() const override final { return false; }

private:
  /// Input COOL folder.
  SG::ReadCondHandleKey<AthenaAttributeList> m_lblbFolderInputKey
  { this, "LBLBFolderInputKey", "/TRIGGER/LUMI/LBLB", "" };


  /// Output conditions object.
  SG::WriteCondHandleKey<LBDurationCondData> m_lbDurationOutputKey
  {this, "LBDurationOutputKey", "LBDurationCondData", "" };
};


#endif // not LUMIBLOCKCOMPS_LBDURATIONCONDALG_H
