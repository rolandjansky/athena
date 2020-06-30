// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwEtaoff_v3.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date July, 2006
 * @brief EM calorimeter eta offset (S-shape) corrections, version 3.
 */


#include "CaloSwEtaoff_v3.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <cmath>
#include <cassert>


/**
 * @brief Virtual function for the correction-specific code.
 * @param myctx   ToolWithConstants context.
 * @param cluster The cluster to correct.
 *                It is updated in place.
 * @param elt     The detector description element corresponding
 *                to the cluster location.
 * @param eta     The @f$\eta@f$ coordinate of the cluster, in this sampling.
 * @param adj_eta The @f$\eta@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param phi     The @f$\phi@f$ coordinate of the cluster, in this sampling.
 * @param adj_phi The @f$\phi@f$ adjusted for
 *                any shift between the actual and nominal coordinates.
 *                (This is shifted back to the nominal coordinate system.)
 * @param samp    The calorimeter sampling we're examining.
 *                This is a sampling code as defined by
 *                @c CaloSampling::CaloSample; i.e., it has both
 *                the calorimeter region and sampling encoded.
 */
void CaloSwEtaoff_v3::makeTheCorrection (const Context& myctx,
                                         xAOD::CaloCluster* cluster,
                                         const CaloDetDescrElement* elt,
                                         float eta,
                                         float adj_eta,
                                         float /*phi*/,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample samp) const
{
  // Find u, the normalized displacement of the cluster within the cell.
  // In the range -1...1, with 0 at the center.
  float u = (eta - elt->eta()) / elt->deta() * 2;
  if (elt->eta_raw() < 0)
    u = -u;

  // u can sometimes be outside of the prescribed range, due to DD bugs.
  if (u > 1)
    u = 1;
  else if (u < -1)
    u = -1;

  const CxxUtils::Array<2> regions = m_regions (myctx);
  assert (regions.size(1) == REG_ENTRIES);

  // Look up the region for this correction.  Give up if we're outsize
  // the range of the table.
  float adj_aeta = std::abs (adj_eta);
  int region_ndx = find_region (regions, adj_aeta);
  if (region_ndx < 0)
    return;

  const CxxUtils::Array<4> correction = m_correction (myctx);
  const CxxUtils::Array<2> forms = m_forms (myctx);

  // In a few regions, the fit was done using a cell size different
  // from what we actually have.  Need to recalculate u in this case.
  if (std::abs (regions[region_ndx][REG_CELLSIZE] - elt->deta()) > 1e-3) {
    float cellsize = regions[region_ndx][REG_CELLSIZE];
    u = fmod (adj_aeta, cellsize) / cellsize * 2 - 1;
  }

  // Calculate the correction for each energy.
  float offs = energy_interpolation (cluster->e(),
                                     Builder (correction,
                                              regions,
                                              forms,
                                              adj_aeta,
                                              u,
                                              region_ndx),
                                     m_energies(myctx),
                                     m_energy_degree(myctx));

  // Apply the offset correction to the cluster.
  if (adj_eta < 0)
    offs = -offs;
  cluster->setEta (samp, eta + offs);
}


/**
 * @brief Find the index of the region containing a given @f$|\eta|@f$ value.
 * @param aeta The @f$|\eta|@f$ value to find.
 * @returns The region index, or -1 if @c aeta is outside the table range.
 */
int CaloSwEtaoff_v3::find_region (const CxxUtils::Array<2>& regions,
                                  float aeta) const
{
  // ??? Would a binary search be better here?
  //     The table shouldn't be very large, so I'll leave it like
  //     this for now.
  unsigned int nreg = regions.size();
  for (unsigned int i=0; i < nreg; i++) {
    if (aeta >= regions[i][REG_LO] && aeta < regions[i][REG_HI])
      return i;
  }
  return -1;
}


/**
 * @brief Constructor for energy interpolation table helper class.
 * @param correction Tabulated arrays of function parameters.
 * @param regions Table of regions.
 * @param forms Functional form per region.
 * @param aeta The absolute value of @f$\eta@f$ at which the correction
 *             is being evaluated (in cal-local coordinates).
 * @param u    The fractional offset of the cluster within the cell.
 * @param region_ndx The index of the region within which the
 *             correction is being evaluated.
 */
CaloSwEtaoff_v3::Builder::Builder (const CxxUtils::Array<4>& correction,
                                   const CxxUtils::Array<2>& regions,
                                   const CxxUtils::Array<2>& forms,
                                   float aeta,
                                   float u,
                                   int region_ndx)
  : m_correction (correction),
    m_regions (regions),
    m_forms (forms),
    m_region_ndx (region_ndx),
    m_aeta (aeta),
    m_u (u),
    m_form (regions[region_ndx][REG_FORM])
{
}


/**
 * @brief Calculate the correction for one tabulated energy index.
 * @param energy_ndx The tabulated energy index for which
 *                   the correction should be calculated.
 * @param good[out]  Set to true if the calculation succeeded.
 */
float CaloSwEtaoff_v3::Builder::calculate (int energy_ndx, bool& good) const
{
  // Find the proper array of coefficients.
  CaloRec::Array<2> coef = m_correction[energy_ndx][m_region_ndx];

  // If we don't have coefficients for this energy/region, skip it.
  if (coef[0].end()[-1] == 0) {
    good = false;
    return 0;
  }

  // Which functional form to use?
  int form;
  if (m_forms.size() != 0 && m_forms.size(1) != 0)
    form = m_forms[m_region_ndx][energy_ndx];
  else
    form = m_form;

  // Evaluate the correction!
  good = true;
  float ret = 0;

  // Required just for case 13
  float xlo=0 ;
  float xhi=0 ;
  unsigned int reg_n = m_regions.size();

  switch (form) {
  case 0:
    ret = calc0 (m_aeta, m_u, coef);
    break;
  case 3:
    ret = calc3 (m_aeta, m_u, coef);
    break;
  case 4:
    ret = calc4 (m_aeta, m_u, coef);
    break;
  case 5:
    ret = calc5 (m_aeta, m_u, coef);
    break;
  case 10:
    ret = calc10 (m_aeta, m_u, coef);
    break;
  case 11:
    ret = calc11 (m_aeta, m_u, coef);
    break;
  case 13:

    for (unsigned int i=0; i < reg_n; i++) {
      if (m_aeta >= m_regions[i][REG_LO] && m_aeta < m_regions[i][REG_HI])
      {
         xlo = m_regions[i][REG_LO] ;
         xhi = m_regions[i][REG_HI] ;
      }
    }

    ret = calc13 (m_aeta, m_u, coef, xlo, xhi);
    break;
  default:
    abort();
  }

  // Protection against correction blowing up due to numerical
  // problems in calculating the polynomial interpolation.
  if (fabs(ret) > 0.025) {
    ret = 0;
    good = false;
  }

  return ret;
}


namespace {


/**
 * @brief Evaluate the polynomials for the correction function parameters.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param coef Array of polynomial coefficients for each parameter.
 * @param npar The number of parameters to evaluate.
 * @param pars[out] The evaluated parameters.
 */
void poly_eval (float aeta,
                const CaloRec::Array<2>& coef,
                unsigned int npar,
                double pars[])
{
  assert (npar <= coef.size(0));

  // Take polynomial degree from the array size.
  unsigned int degree = coef.size(1);

  // Loop over parameters.
  for (unsigned int i=0; i < npar; i++) {
    // Coefficients for this parameter.
    CaloRec::Array<1> xcoef = coef[i];

    // Evaluate polynomial in @c aeta with coefficients @c xcoef.
    double out = xcoef[0];
    for (unsigned int j = 1; j < degree; j++)
      out = out * aeta + xcoef[j];
    pars[i] = out;
  }
}


} // anonymous namespace


/**
 * @brief Evaluate the correction function for Form 0.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc0 (float aeta,
                                       float u,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 5;
  double pars[NPAR];
  poly_eval (aeta, coef, NPAR, pars);
  double c = (u>=0) ? pars[2] : pars[3];
  return pars[0] * (std::atan (pars[1] * u) + c*u + pars[4]);
}


/**
 * @brief Evaluate the correction function for Form 3.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc3 (float aeta,
                                       float u,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 4;
  double pars[NPAR];
  poly_eval (aeta, coef, NPAR, pars);
  double c = pars[2];
  if (u < 0)
    c = -c - 2*std::atan(pars[1]);
  return pars[0] * (std::atan (pars[1] * u) + c*u + pars[3]);
}


/**
 * @brief Evaluate the correction function for Form 4.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc4 (float aeta,
                                       float u,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 3;
  double pars[NPAR];
  poly_eval (aeta, coef, NPAR, pars);
  double b = std::max ((double)pars[1], 1e-5);
  double atanb = std::atan(b);
  double sq = std::sqrt (b/atanb - 1);
  double den = (sq/b*atanb - std::atan(sq));
  return  pars[0]* ((- std::atan (b*u) + u*atanb) / den +
                    pars[2]*(1-std::abs(u)));
}


/**
 * @brief Evaluate the correction function for Form 4.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc5 (float aeta,
                                       float /*u*/,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 1;
  double pars[NPAR];
  poly_eval (aeta, coef, NPAR, pars);
  return pars[0];
}


/**
 * @brief Evaluate the correction function for Form 10.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc10 (float aeta,
                                        float u,
                                        const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 7;
  double pars[NPAR];
  poly_eval (aeta, coef, NPAR, pars);
  double c = pars[2];
  if (u < 0)
    c = -c - 2*std::atan(pars[1]);

  double offs = pars[0] * (atan (pars[1] * u) + c*u + pars[3]) +
    pars[4] * std::cos (u * pars[5] * M_PI + pars[6]);
  if (u > 0.9 || u < -0.9) {
    double xdiff = pars[4] * (std::cos (pars[5] * M_PI + pars[6]) -
                              std::cos (- pars[5] * M_PI + pars[6]));
    if (u > 0.9)
      offs -= (u-0.9)*(1./0.1) * xdiff/2;
    else
      offs += (-0.9-u)*(1./0.1) * xdiff/2;
  }
  return offs;
}


/**
 * @brief Evaluate the correction function for Form 11.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc11 (float aeta,
                                        float u,
                                        const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 8;
  double pars[NPAR];
  poly_eval (aeta, coef, NPAR, pars);
  double c = (u>=0) ? pars[2] : pars[3];
  return pars[0] * (std::atan (pars[1] * u) + c*u + pars[4])
    + pars[5] * std::cos (u * pars[6] * M_PI + pars[7]);
}


/**
 * @brief Evaluate the correction function for Form 5.
 * @param aeta The @f$|\eta|@f$ of the cluster being corrected.
 * @param u    The fractional offset of the cluster within the cell.
 * @param coef Array of polynomial coefficients for each parameter.
 */
float CaloSwEtaoff_v3::Builder::calc13 (float aeta,
                                        float /*u*/,
                                        const CaloRec::Array<2>& coef,
                                        float xlo,
                                        float xhi) const
{
  const unsigned int NPAR = 1;
  double pars[NPAR];
  float x = (aeta - xlo) / (xhi - xlo) ;
  poly_eval (x, coef, NPAR, pars);
  return pars[0];
}


/*
from python code for writing calc 13

# D3PD edit - udpated by Ewan since Scott only half implemented it
# "Scaled Polynomial" fit function
def _calc13 (aeta, u, ptab, m_xlo, m_xhi):
    x = (aeta - m_xlo) / (m_xhi - m_xlo);
    p = [_poly_eval (x, pp) for pp in ptab]
    return p[0]
*/
