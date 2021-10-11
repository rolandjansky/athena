// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloLumiConditions/CaloBCIDCoeffs.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief Luminosity-dependent pileup offset correction conditions object.
 */


#ifndef CALOLUMICONDITIONS_CALOBCIDCOEFFS_H
#define CALOLUMICONDITIONS_CALOBCIDCOEFFS_H


#include "CxxUtils/aligned_vector.h"
#include "Identifier/HWIdentifier.h"
#include <vector>

class ILArOFC;
class ILArShape;
class ILArMinBiasAverage;
class LArOnlineID_Base;


/**
 * @brief Luminosity-dependent pileup offset correction conditions object.
 *
 * This implements the cell-by-cell luminosity-dependent offset correction
 * for adjacent bunches.  This was previously implemented as an algorithm
 * (CaloBCIDAvgAlg), but it took too long to be usable in the trigger.
 * We can, however, speed it up drastically by preformatting the
 * constant data and saving it in a conditions object.  The pieces
 * of that related to calibration are stored here.  The per-bunch luminosity
 * information is stored in a separate conditions object, as it has
 * a different lifetime.
 *
 * The correction is carried out for each cell.  Symmetry is taken
 * into account via LArMCSym, and the calculation is done for only
 * one cell in each symmetry group, which results in a total of 1833 cells.
 * The calculation depends on the per-cell pulse shape S_j (j=0..N_S-1),
 * the per-cell optimial-filter coeffients O_i (i=0..N_O-1),
 * the per-cell average minibias M, and the per-bunch luminosity
 * L_k (k=0..NBCID-1).  It can be written as:
 *
 *   Offset = M \Sum_i O_i \Sum_j S_j L_(BCID+i-j)
 *
 * where the luminosity array L is taken to be circular
 * (i.e., the indexing is mod NBCID).  (In cases with four samples,
 * the luminosity index is offset by 1 for HEC cells.)
 *
 * This can be rewritten grouping together terms which access
 * the same L index:
 *
 *  Offset = \Sum_m (\Sum_l M O_(m+l) S_l )  L_(BCID+m)
 *
 * Everything inside the parentheses can be precomputed, reducing the
 * calculation to a dot product between a vector of constants and
 * an appropriate subrange of the luminosity vector.
 * The circular nature of the luminosity vector can be handled
 * by padding the beginning and end and duplicating the values there.
 * The special case mentioned above for four samples is handled by
 * treating things as if we actually have five samples and padding
 * the OFC coefficients with a zero at either the beginning or end
 * (depending on whether the cell is HEC).
 *
 * The S_j values are not positive definite (the shape is normalized to zero).
 * So when we do the sums above, we're adding terms that are both positive
 * and negative.  This can result in some loss of precision, and means that
 * when we reorder the sum as above, we shouldn't expect to get exactly
 * the same result.
 *
 * This calculation vectorizes well.  It is better to vectorize over cells,
 * to avoid dependencies between steps.  We always do this in groups of 8
 * (regardless of platform) so that sums are always in the same order.
 * This means that we need to pad things out so that the number of cells
 * is a multiple of 8.
 *
 * The optimized and vectorized calculation is found to be about a factor
 * of 100 faster than the original naive implementation.
 */
class CaloBCIDCoeffs
{
public:
  /** 
   * @brief Constructor.
   * @param hwids List of cell HWIDs for which we need to do the calculation.
   * @param online_id Online ID helper.
   * @param ofcs OFC coefficient conditions object.
   * @param shapes Pulse shape conditions object.
   * @param minbias Minimum bias conditions object.
   */
  CaloBCIDCoeffs (const std::vector<HWIdentifier>& hwids,
                  const LArOnlineID_Base& online_id,
                  const ILArOFC& ofcs,
                  const ILArShape& shapes,
                  const ILArMinBiasAverage& minbias);


  /** 
   * @brief Perform the calculation for a given set of per-bunch luminosities.
   * @param lumi Per-bunch luminosities.
   *             Should be properly offset in the BCID vector.
   *             Will access lumi[-(nshapes-1)] to lumi[nsamples-1],
   *             inclusive.
   * @param out Output per-cell offsets.
   */
  void calc (const float* lumi,
             CxxUtils::vec_aligned_vector<float>& out) const;


  /**
   * @brief Return the number of shape points per cell.
   */
  size_t nshapes() const;


  /**
   * @brief Return the number of samples per cell used in the calculation
   *        (after padding).
   */
  size_t nsamples_coeff() const;


private:
  /// Number of cells that we calculate at one time.
  static constexpr size_t CHUNKSIZE = 8;

  /// Number of cells.
  unsigned int m_ncell;

  /// Number of padding cells we need to add to get a multiple of CHUNKSIZE.
  unsigned int m_npad;

  /// Number of coefficients per cell (length of dot product).
  unsigned int m_ncoeff;

  /// Number of samples per cell (length of OFC vector).
  unsigned int m_nsamples;

  /// Number of samples per cell used in the calculation, after padding.
  unsigned int m_nsamples_coeff;

  /// Number of shape points per cell.
  unsigned int m_nshapes;

  /// Storage for coeffients, m_ncoeff per cell,
  /// laid out in groups of CHUNKSIZE like this:
  /// icoeff0, icell 0
  ///   ...
  /// icoeff0, icell CHUNKSIZE-1
  /// icoeff1, icell 0
  /// ...
  /// icoeff(m_ncoeff-1) icell CHUNKSIZE-1
  /// icoeff0, icell CHUNKSIZE
  /// ...
  CxxUtils::vec_aligned_vector<float> m_coeffs;


  /**
   * @brief Indexing into m_coeffs.
   * @param icoeff Coeffienct index.
   * @param icell Cell index.
   */
  float& coeff (size_t icoeff, size_t icell)
  {
    size_t chunk = icell / CHUNKSIZE;
    size_t ndx = chunk * m_ncoeff * CHUNKSIZE + icoeff * CHUNKSIZE + (icell%CHUNKSIZE);
    return m_coeffs[ndx];
  }


  /** 
   * @brief Initialize all coefficients.
   * @param hwids List of cell HWIDs for which we need to do the calculation.
   * @param online_id Online ID helper.
   * @param ofcs OFC coefficient conditions object.
   * @param shapes Pulse shape conditions object.
   * @param minbias Minimum bias conditions object.
   */
  void fillCoeffs (const std::vector<HWIdentifier>& hwids,
                   const LArOnlineID_Base& online_id,
                   const ILArOFC& ofcs,
                   const ILArShape& shapes,
                   const ILArMinBiasAverage& minbias);


  /** 
   * @brief Find coefficients for one cell.
   * @param ofcs Array of OFC coefficients for this cell.
   * @param shapes Array of shape points for this cell.
   * @param ishec True if this is a HEC cell.
   * @param cell_coeffs Output coefficient array.
   */
  void findCellCoeffs (const float* ofcs,
                       const float* shapes,
                       bool ishec,
                       std::vector<float>& cell_coeffs) const;
  
};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CaloBCIDCoeffs , 18590337 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(CaloBCIDCoeffs, 256004943);


/**
 * @brief Return the number of shape points per cell.
 */
inline
size_t CaloBCIDCoeffs::nshapes() const
{
  return m_nshapes;
}


/**
 * @brief Return the number of samples per cell used in the calculation
 *        (after padding).
 */
inline
size_t CaloBCIDCoeffs::nsamples_coeff() const
{
  return m_nsamples_coeff;
}


#endif // not CALOLUMICONDITIONS_CALOBCIDCOEFFS_H
