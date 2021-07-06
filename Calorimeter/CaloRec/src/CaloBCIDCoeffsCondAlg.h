// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloBCIDCoeffsCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Conditions algorithm to create CaloBCIDCoeffs.
 */


#ifndef CALOREC_CALOBCIDCOEFFSCONDALG_H
#define CALOREC_CALOBCIDCOEFFSCONDALG_H


#include "CaloLumiConditions/CaloBCIDCoeffs.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArRawConditions/LArMCSym.h"
#include "LArIdentifier/LArOnlineID_Base.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Conditions algorithm to create CaloBCIDCoeffs.
 *
 * This is part of the cell-by-cell bunch-dependent pileup offset correction.
 * The information needed to do the calculation is stored in two
 * conditions objects, CaloBCIDCoeffs and CaloBCIDLumi.
 * The former has calibration-related values, processed to enable
 * efficient calculation, and the latter has the per-bunch
 * luminosities.  There are two distinct conditions objects because
 * the luminosity changes much faster than then calibrations.
 */
class CaloBCIDCoeffsCondAlg : public AthReentrantAlgorithm
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
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  /// Property: Symmetrization helper (conditions input).
  SG::ReadCondHandleKey<LArMCSym> m_mcSymKey
  {this, "MCSymKey", "LArMCSym", "SG Key of LArMCSym object"};

  /// Property: OFC coefficients (conditions input).
  SG::ReadCondHandleKey<ILArOFC> m_ofcKey
  {this, "OFCKey", "LArOFC", "SG Key of OFC conditions object" };

  /// Property: Pulse shape (conditions input).
  SG::ReadCondHandleKey<ILArShape> m_shapeKey
  {this, "ShapeKey", "LArShape32", "SG Key of Shape conditions object"};

  /// Property: Min bias offset (conditions input).
  SG::ReadCondHandleKey<ILArMinBiasAverage> m_minBiasAvgKey
  {this, "MinBiasAvgKey", "LArPileupAverageSym", "SGKey of LArMinBiasAverage object"};

  /// Property: Offset calculation coefficients (conditions output).
  SG::WriteCondHandleKey<CaloBCIDCoeffs> m_outputCoeffsKey
  { this, "OutputCoeffsKey", "CaloBCIDCoeffs", "SG key of output coefficients" };

  /// Property: m_isSC
  Gaudi::Property<bool> m_isSC { this, "IsSuperCell",false,"Is SuperCell" };

  /// LAr online ID helper.
  const LArOnlineID_Base* m_laronline_id = nullptr;
};


#endif // not CALOREC_CALOBCIDCOEFFSCONDALG_H
