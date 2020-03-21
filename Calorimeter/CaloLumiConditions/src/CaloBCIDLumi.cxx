/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloLumiConditions/src/CaloBCIDLumi.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Luminosity-dependent pileup offset correction conditions object.
 */


#include "CaloLumiConditions/CaloBCIDLumi.h"
#include "CaloLumiConditions/CaloBCIDCoeffs.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockData/BunchCrossingCondData.h"


/** 
 * @brief Constructor (for MC).
 * @param coeffs Corresponding coefficiencts conditions object.
 * @param bcData Flags of occupied bunches.
 */
CaloBCIDLumi::CaloBCIDLumi (const CaloBCIDCoeffs& coeffs,
                            const BunchCrossingCondData& bcData)
  : m_coeffs (coeffs)
{
  // MC case
  // convert from mu/bunch to lumi in 10**30 units per bunch (for MC only)
  // 25ns*Nbcid*71mb*10**30 = 25e-9*3564*71e-27*1e30 = 6.31 Use 1/6.31 = 0.158478605
  // For this implementation, we multiply through by
  // averageInteractionsByCrossing() in calc().
  const float xlumiMC = /*ei->averageInteractionsPerCrossing()* */ 0.158478605;

  static constexpr int MAX_BCID = BunchCrossingCondData::m_MAX_BCID;

  std::vector<float> lumiVec (MAX_BCID);
  for (size_t i = 0; i < MAX_BCID; i++) {
    lumiVec[i] = bcData.isFilled(i)*xlumiMC; 
  }

  initLumi (lumiVec);
}


/** 
 * @brief Constructor (for data).
 * @param coeffs Corresponding coefficiencts conditions object.
 * @param lumiData Per-bunch luminosity information.
 */
CaloBCIDLumi::CaloBCIDLumi (const CaloBCIDCoeffs& coeffs,
                            const LuminosityCondData& lumiData)
  : m_coeffs (coeffs)
{
  initLumi (lumiData.lbLuminosityPerBCIDVector());
}


/**
 * @brief Initialize @c m_lumiData.
 * @param lumiVec Per-BCID luminosities.
 */
void CaloBCIDLumi::initLumi (const std::vector<float>& lumiVec)
{
  // Number of entries from the end we put at the beginning.
  unsigned int nbefore = m_coeffs.nshapes()-1;
  // Number of entries from the beginning we put at the end.
  unsigned int nafter = m_coeffs.nsamples_coeff()-1;

  m_lumiData.resize (nbefore + lumiVec.size() + nafter);

  // Copy [BCID-nbefore, NBCID) to the start of m_lumiData.
  std::copy (lumiVec.end() - nbefore, lumiVec.end(),
             m_lumiData.begin());

  // Append [0, NBCID)
  std::copy (lumiVec.begin(), lumiVec.end(),
             m_lumiData.begin() + nbefore);

  // Append [0, nafter)
  std::copy (lumiVec.begin(), lumiVec.begin() + nafter,
             m_lumiData.begin() + nbefore + lumiVec.size());

  // Set the pointer to BCID 0.
  m_lumi = m_lumiData.data() + nbefore;
}


/** 
 * @brief Perform the calculation for a given BCID.
 * @param bcid Desired BCID.
 * @param averageInteractionsPerCrossing Scale to be applied to all outputs.
 * @param out Output per-cell offsets.
 */
void CaloBCIDLumi::calc (const size_t bcid,
                         const float averageInteractionsPerCrossing,
                         CxxUtils::vec_aligned_vector<float>& out) const
{
  // Perform the calculation using the proper lumi range for the BCID.
  m_coeffs.calc (m_lumi + bcid, out);

  // Scale the results if requested (for MC).
  if (averageInteractionsPerCrossing != 1) {
    for (float& o : out) {
      o *= averageInteractionsPerCrossing;
    }
  }
}
