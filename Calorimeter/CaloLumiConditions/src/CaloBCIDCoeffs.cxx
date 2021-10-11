/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloLumiConditions/src/CaloBCIDCoeffs.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief Luminosity-dependent pileup offset correction conditions object.
 */


#include "CaloLumiConditions/CaloBCIDCoeffs.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CxxUtils/vec.h"
#include "CxxUtils/features.h"


/** 
 * @brief Constructor.
 * @param hwids List of cell HWIDs for which we need to do the calculation.
 * @param online_id Online ID helper.
 * @param ofcs OFC coefficient conditions object.
 * @param shapes Pulse shape conditions object.
 * @param minbias Minimum bias conditions object.
 */
CaloBCIDCoeffs::CaloBCIDCoeffs (const std::vector<HWIdentifier>& hwids,
                                const LArOnlineID_Base& online_id,
                                const ILArOFC& ofcs,
                                const ILArShape& shapes,
                                const ILArMinBiasAverage& minbias)
{
  // Number of cells we're calculating.
  m_ncell = hwids.size();

  // Number of cells of padding we need to add to get a multiple of CHUNKSIZE.
  m_npad = CHUNKSIZE - (m_ncell % CHUNKSIZE);
  if (m_npad == CHUNKSIZE) m_npad = 0;

  // Number of shape / samples.
  if (hwids.empty()) {
    m_nsamples = 0;
    m_nshapes = 0;
  }
  else {
    m_nsamples = ofcs.OFC_a (hwids[0], 0).size();
    m_nshapes = shapes.Shape (hwids[0], 0).size();
  }

  // Special case for 4 samples: treat this case as if we actually
  // had 5 samples, just with 0-padding.
  m_nsamples_coeff = m_nsamples;
  if (m_nsamples == 4) {
    m_nsamples_coeff = 5;
  }

  // Number of coefficients (length of the dot product).
  m_ncoeff = (m_nshapes-1) + 1 + (m_nsamples_coeff-1);

  // Initialize coeffients.
  fillCoeffs (hwids, online_id, ofcs, shapes, minbias);
}


/** 
 * @brief Initialize all coefficients.
 * @param hwids List of cell HWIDs for which we need to do the calculation.
 * @param online_id Online ID helper.
 * @param ofcs OFC coefficient conditions object.
 * @param shapes Pulse shape conditions object.
 * @param minbias Minimum bias conditions object.
 */
void CaloBCIDCoeffs::fillCoeffs (const std::vector<HWIdentifier>& hwids,
                                 const LArOnlineID_Base& online_id,
                                 const ILArOFC& ofcs,
                                 const ILArShape& shapes,
                                 const ILArMinBiasAverage& minbias)
{
  // Reserve space for all coefficients.
  m_coeffs.resize (m_ncoeff * (m_ncell + m_npad));

  // Temporary vector to hold coefficients for one cell.
  // Declare outside the loop to reduce allocations.
  std::vector<float> cell_coeffs (m_ncoeff);

  for (size_t icell = 0; icell < m_ncell; ++icell) {
    HWIdentifier hwid = hwids[icell];
    float cell_minbias = minbias.minBiasAverage (hwid);

    // If cell_minbias <= 0, we want the result to be 0.
    // So just leave the coefficients as 0 in that case.
    if (cell_minbias > 0) {
      // Calculate coefficients for one cell.
      findCellCoeffs (ofcs.OFC_a (hwid, 0).data(),
                      shapes.Shape (hwid, 0).data(),
                      online_id.isHECchannel (hwid),
                      cell_coeffs);

      // Enter them into the coefficients array in the proper places.
      // Here, we also multiply everything by minbias.
      for (size_t icoeff = 0; icoeff < m_ncoeff; ++icoeff) {
        coeff(icoeff, icell) = cell_coeffs[icoeff] * cell_minbias;
      }
    }
  }
}


/** 
 * @brief Find coefficients for one cell.
 * @param ofcs Array of OFC coefficients for this cell.
 * @param shapes Array of shape points for this cell.
 * @param ishec True if this is a HEC cell.
 * @param cell_coeffs Output coefficient array.
 */
void CaloBCIDCoeffs::findCellCoeffs (const float* ofcs,
                                     const float* shapes,
                                     bool ishec,
                                     std::vector<float>& cell_coeffs) const
{
  // Initialize output to zero.
  std::fill (cell_coeffs.begin(), cell_coeffs.end(), 0);

  const unsigned nsamples_coeff = m_nsamples_coeff;
  const unsigned nshapes = m_nshapes;

  // Loop over all combinations of sample / shape.
  for (size_t i = 0; i < nsamples_coeff; ++i) {
    for (size_t j = 0; j < nshapes; ++j) {
      // Index of coefficient we're filling.
      size_t ndx = i - j + (nshapes-1);

      // Find OFC coefficient.
      float ofc = 0;
      if (m_nsamples == 4)
      {
        // Special case for 4 samples: we actually calclate with five samples,
        // but pad with 0 at either the beginning or end (depending on
        // whether this is a HEC cell).
        if (ishec) {
          if (i > 0) ofc = ofcs[i-1];
        }
        else {
          if (i < 4) ofc = ofcs[i];
        }
      }
      else {
        ofc = ofcs[i];
      }

      // Do the multiplication and sum.
      cell_coeffs.at(ndx) += ofc * shapes[j];
    }
  }
}


/** 
 * @brief Perform the calculation for a given set of per-bunch luminosities.
 * @param lum Per-bunch luminosities.
 *            Should be properly offset in the BCID vector.
 *            Will access lumi[-(nshapes-1)] to lumi[nsamples-1],
 *            inclusive.
 * @param out Output per-cell offsets.
 */
#if defined(__x86_64__) && HAVE_TARGET_CLONES && HAVE_VECTOR_SIZE_ATTRIBUTE
// If we have function multiversioning, compile specialized versions
// for different architectures.
// ... But including avx causes a crash at least with gcc 8.3.0.
//     It works ok with gcc 10.  (Not tested with 9.)
#if __GNUC__ == 8
__attribute__ ((target_clones ("default,sse2"))) 
#else
__attribute__ ((target_clones ("default,sse2,avx,avx2"))) 
#endif
#endif
void CaloBCIDCoeffs::calc  (const float* lumi,
                            CxxUtils::vec_aligned_vector<float>& out) const
{
  // Vector type.
  typedef CxxUtils::vec<float, CHUNKSIZE> vf;

  const unsigned ncell = m_ncell;
  const unsigned ncoeff = m_ncoeff;
  const unsigned int nshapes = m_nshapes;

  // Broadcast the luminosity values to all vector lanes.
  CxxUtils::vec_aligned_vector<vf> llv (ncoeff);
  for (size_t i = 0; i < ncoeff; i++) {
    CxxUtils::vbroadcast (llv[i], lumi[i - (nshapes-1)]);
  }

  // Size the output array, including padding to make it a multiple
  // of CHUNKSIZE.
  out.resize (ncell + m_npad);

  // Pointers iterating through the coefficients and the output.
  const vf* cpos = reinterpret_cast<const vf*> (m_coeffs.data());
  vf* opos = reinterpret_cast<vf*> (out.data());

  // Loop over cells in groups of CHUNKSIZE.
  for (size_t icell = 0; icell < ncell; icell += CHUNKSIZE) {

    // Loop over coefficients and calculate the dot product.
    // It is more efficient to keep three separate accumulators
    // and add them together at the end --- probably because that
    // allows memory accesses to overlap with calculations.
    // There was no significant observed benefit to using
    // more than three accumulators.

    // Prepare to loop through luminosity values.
    vf* ll = llv.data();

    // Initialize three accumulators.
    vf sum1 = {0};
    vf sum2 = {0};
    vf sum3 = {0};

    // Loop accumulating the dot product in three pieces.
    int i = ncoeff;
    for (; i >= 3; i -= 3) {
      sum1 += *cpos++ * *ll++;
      sum2 += *cpos++ * *ll++;
      sum3 += *cpos++ * *ll++;
    }

    // Handle any trailing values if the number of coefficients is not
    // divisible by three.  (Would not usually be executed as we expect
    // to usually have 36 coefficients.)
    switch (i) {
    case 2:
      sum1 += *cpos++ * *ll++;
      [[fallthrough]];
    case 1:
      sum2 += *cpos++ * *ll++;
      break;
    default:
      break;
    }

    // Sum the three accumulators and write CHUNKSIZE cells to the output.
    *opos++ = sum1 + sum2 + sum3;
  }

  // Remove padding from the end of the output vector.
  out.resize (ncell);
}
