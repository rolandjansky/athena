// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloBCIDLumiCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Conditions algorithm to create CaloBCIDLumi.
 */


#ifndef CALOREC_CALOBCIDLUMICONDALG_H
#define CALOREC_CALOBCIDLUMICONDALG_H


#include "CaloLumiConditions/CaloBCIDLumi.h"
#include "CaloLumiConditions/CaloBCIDCoeffs.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockData/BunchCrossingCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Conditions algorithm to create CaloBCIDLumi.
 *
 * This is part of the cell-by-cell bunch-dependent pileup offset correction.
 * The information needed to do the calculation is stored in two
 * conditions objects, CaloBCIDCoeffs and CaloBCIDLumi.
 * The former has calibration-related values, processed to enable
 * efficient calculation, and the latter has the per-bunch
 * luminosities.  There are two distinct conditions objects because
 * the luminosity changes much faster than then calibrations.
 */
class CaloBCIDLumiCondAlg : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Execute the algorithm.
   * @param ctx Event context.
   */
  virtual StatusCode execute (const EventContext& ctx) const override final;
  virtual bool isReEntrant() const override final { return false; }
private:
  /// Property: Coefficients object (conditions input).
  SG::ReadCondHandleKey<CaloBCIDCoeffs> m_coeffsKey
  {this, "CoeffsKey", "CaloBCIDCoeffs", "SG Key coefficient object"};

  /// Property: Bunch crossing data (MC only) (conditions input).
  SG::ReadCondHandleKey<BunchCrossingCondData> m_bcDataKey
  {this, "BunchCrossingCondDataKey", "BunchCrossingData" ,"SG Key of BunchCrossing CDO"};

  /// Property: Per-bunch luminosity data (data only) conditions input).
  SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey
  {this, "LuminosityCondDataKey", "LuminosityCondData", "SG Key of LuminosityCondData object"};

  /// Property: Offset correction luminosity-dependent conditions object (output).
  SG::WriteCondHandleKey<CaloBCIDLumi> m_outputLumiKey
  { this, "OutputLumiKey", "CaloBCIDLumi", "SG key of output luminosity object" };

  /// Property; MC flag.
  Gaudi::Property<bool> m_isMC
  {this, "isMC", false, "Real data or MC"};
};


#endif // not CALOREC_CALOBCIDLUMICONDALG_H
