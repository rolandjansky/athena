// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/src/TrigLiveFractionCondAlg.h
 * @author scott snyder <snyder@bnl.gov>, from existing TrigLivefractionTool
 * @date Jun, 2019
 * @brief Conditions algorithm for trigger live fraction data.
 */


#ifndef LUMIBLOCKCOMPS_TRIGLIVEFRACTIONCONDALG_H
#define LUMIBLOCKCOMPS_TRIGLIVEFRACTIONCONDALG_H


#include "LumiBlockData/TrigLiveFractionCondData.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "CoralBase/Blob.h"


/**
 * @brief Conditions algorithm for trigger live fraction data.
 */
class TrigLiveFractionCondAlg
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
  /**
   * @brief Unpack information from the deadtime COOL folder.
   * @param deadtimeFolder Input COOL folder.
   * @param turnCounter[out] Number of turns for this IOV.
   * @param liveFractionLo[out] Per-BCID live fractions, low-priority
   * @param liveFractionHi[out] Per-BCID live fractions, high-priority
   */
  StatusCode
  unpackDeadtime (const AthenaAttributeList& deadtimeFolder,
                  unsigned int& turnCounter,
                  std::vector<float>& liveFractionLo,
                  std::vector<float>& liveFractionHi) const;


  /**
   * @brief Unpack a single deadtime vector.
   * @param blob Input data blob
   * @param turnCounter Number of turns in this IOV.
   * @param livevec[out] Per-BCID live fractions, low-priority
   *        Should already be sized to TOTAL_LHC_BCIDS.
   */
  StatusCode unpackVector  (const coral::Blob& blob,
                            unsigned int turnCounter,
                            std::vector<float>& livevec) const;


  /**
   * @brief Calculate weighted average live fractions.
   * @param lumi Luminosity info.
   * @param liveFractionLo Per-BCID live fractions, low-priority
   * @param liveFractionHi Per-BCID live fractions, high-priority
   * @param lumiLiveFractionLo[out] Weighted average live fractions, low-prio.
   * @param lumiLiveFractionHi[out] Weighted average live fractions, high-prio.
   */
  static StatusCode lumiWeight  (const LuminosityCondData& lumi,
                          const std::vector<float>& liveFractionLo,
                          const std::vector<float>& liveFractionHi,
                          float& lumiLiveFractionLo,
                          float& lumiLiveFractionHi) ;

  
  /// Input deadtime folder.
  SG::ReadCondHandleKey<AthenaAttributeList> m_deadtimeFolderInputKey
  { this, "DeadtimeFolderInputKey", "/TRIGGER/LUMI/PerBcidDeadtime",
      "Input deadtime COOL folder." };

  /// Input luminosity data.
  SG::ReadCondHandleKey<LuminosityCondData> m_luminosityInputKey
  { this, "LuminosityInputKey", "LuminosityCondData",
      "Input luminosity data." };

  /// Output conditions object.
  SG::WriteCondHandleKey<TrigLiveFractionCondData> m_trigLiveFractionOutputKey
  { this, "TrigLiveFractionOutputKey", "TrigLiveFractionCondData",
    "Output live fraction data." };
};


#endif // not LUMIBLOCKCOMPS_TRIGLIVEFRACTIONCONDALG_H
