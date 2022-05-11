// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloLumiConditions/CaloBCIDLumi.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Luminosity-dependent pileup offset correction conditions object.
 */


#ifndef CALOLUMICONDITIONS_CALOBCIDLUMI_H
#define CALOLUMICONDITIONS_CALOBCIDLUMI_H


#include "CxxUtils/aligned_vector.h"
#include <vector>


class CaloBCIDCoeffs;
class BunchCrossingCondData;
class LuminosityCondData;


/**
 * @brief Luminosity-dependent pileup offset correction conditions object.
 *
 * This implements the cell-by-cell luminosity-dependent offset correction
 * for adjacent bunches.  This was previously implemented as an algorithm
 * (CaloBCIDAvgAlg), but it took too long to be usable in the trigger.
 * We can, however, speed it up drastically by preformatting the
 * constant data and saving it in a conditions object.  The per-bunch
 * luminosity data are stored here.  The processed calibration/shape
 * information is stored in a separate conditions object CaloBCIDCoeffs,
 * as it has a different lifetime.
 *
 * The per-bunch luminosity vector is considered to be circular.
 * To make this more efficient, we allocate a bit more than is needed,
 * and duplicate the entries at the ends:
 *
 *     bcid[NBCID-nbefore, NBCID)  bcid[0, NBCID)  bcid [0, nafter)
 *
 * where nbefore is nshapes-1 and nafter is nsamples_coeff-1.
 */
class CaloBCIDLumi
{
public:
  /** 
   * @brief Constructor (for MC).
   * @param coeffs Corresponding coefficiencts conditions object.
   * @param bcData Flags of occupied bunches.
   */
  CaloBCIDLumi (const CaloBCIDCoeffs& coeffs,
                const BunchCrossingCondData& bcData);


  /** 
   * @brief Constructor (for data).
   * @param coeffs Corresponding coefficiencts conditions object.
   * @param lumiData Per-bunch luminosity information.
   */
  CaloBCIDLumi (const CaloBCIDCoeffs& coeffs,
                const LuminosityCondData& lumiData);


  /** 
   * @brief Perform the calculation for a given BCID.
   * @param bcid Desired BCID.
   * @param averageInteractionsPerCrossing Scale to be applied to all outputs.
   * @param out Output per-cell offsets.
   */
  void calc (const size_t bcid,
             const float averageInteractionsPerCrossing,
             CxxUtils::vec_aligned_vector<float>& out) const;


private:
  /**
   * @brief Initialize @c m_lumiData.
   * @param lumiVec Per-BCID luminosities.
   */
  void initLumi (const std::vector<float>& lumiVec);


  /// Per-BCID luminosities.  Padded at the start and end as described above.
  std::vector<float> m_lumiData;


  /// Pointer to the luminosity data for BCID 0 (after initial padding).
  const float* m_lumi = nullptr;


  /// Associated coefficients conditions object.
  const CaloBCIDCoeffs& m_coeffs;
};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CaloBCIDLumi , 184361154 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_MIXED_DEF(CaloBCIDLumi, 5332886);


#endif // not CALOLUMICONDITIONS_CALOBCIDLUMI_H
