// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/OnlineLumiCalibrationCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief Produce lumi calibration data from COOL.
 */


#ifndef COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDALG_H
#define COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDALG_H


#include "CoolLumiUtilities/OnlineLumiCalibrationCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Produce lumi calibration data from COOL.
 */
class OnlineLumiCalibrationCondAlg
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
  SG::ReadCondHandleKey<CondAttrListCollection> m_calibrationFolderInputKey
  { this, "CalibrationFolderInputKey", "", "" };

  /// Output conditions object.
  SG::WriteCondHandleKey<OnlineLumiCalibrationCondData> m_lumiCalibOutputKey
  {this, "LumiCalibOutputKey", "OnlineLumiCalibrationCondData", "" };
};


#endif // not COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDALG_H

