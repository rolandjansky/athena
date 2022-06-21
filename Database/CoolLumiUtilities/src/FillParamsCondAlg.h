// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/FillParamsCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Conditions algorithm to unpack fill parameters from COOL.
 */


#ifndef COOLLUMIUTILITIES_FILLPARAMSCONDALG_H
#define COOLLUMIUTILITIES_FILLPARAMSCONDALG_H


#include "CoolLumiUtilities/FillParamsCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Conditions algorithm to unpack fill parameters from COOL.
 */
class FillParamsCondAlg
  : public AthReentrantAlgorithm
{
public:
  /// Forward base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /// Gaudi initialize method.
  virtual StatusCode initialize() override final;


  /// Algorithm execute method.
  virtual StatusCode execute (const EventContext& ctx) const override final;
  virtual bool isReEntrant() const override final { return false; }

private:
  /// Input conditions object.
  SG::ReadCondHandleKey<AthenaAttributeList> m_fillParamsFolderInputKey
  { this, "FillParamsFolderInputKey", "", "" };

  /// Output conditions object.
  SG::WriteCondHandleKey<FillParamsCondData> m_fillParamsOutputKey
  {this, "FillParamsOutputKey", "FillParamsCondData", "" };
};


#endif // not COOLLUMIUTILITIES_FILLPARAMSCONDALG_H
