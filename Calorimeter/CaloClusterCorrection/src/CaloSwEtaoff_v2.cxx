// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwEtaoff_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date July, 2004
 * @brief EM calorimeter eta offset (S-shape) corrections.
 */


#include "CaloSwEtaoff_v2.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include <cmath>
#include <cassert>


using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;


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
void CaloSwEtaoff_v2::makeTheCorrection (const Context& myctx,
                                         CaloCluster* cluster,
                                         const CaloDetDescrElement* elt,
                                         float eta,
                                         float adj_eta,
                                         float /*phi*/,
                                         float /*adj_phi*/,
                                         CaloSampling::CaloSample samp) const
{
  const CxxUtils::Array<3> correction = m_correction (myctx);
  const CxxUtils::Array<1> interp_barriers = m_interp_barriers (myctx);
  const CxxUtils::Array<1> energies = m_energies (myctx);
  const int degree = m_degree (myctx);
  const int energy_degree = m_energy_degree (myctx);

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

  // For each energy, interpolate in eta.
  // We can't use the common energy_interpolation code here,
  // because we're interpolating the fit parameters, not the overall
  // correction.  This should probably be done differently in the
  // next version.
  unsigned int n_energies = energies.size();
  unsigned int shape[] = {n_energies, 4};
  CaloRec::WritableArrayData<2> partab (shape);

  // If we're outside the range of the table, we'll just be using the
  // value at the end (no extrapolation).  We only need to calculate
  // that one point in that case.
  int beg = 0;
  int end = n_energies;
  float energy = cluster->e();
  if (energy <= energies[0])
    end = 1;
  else if (energy >= energies[n_energies-1])
    beg = n_energies-1;

  for (int i=beg; i<end; i++) {
    partab[i][0] = energies[i];
    for (int j=0; j < 3; j++)
      partab[i][j+1] = interpolate (correction[i],
                                    std::abs (adj_eta), 
                                    degree,
                                    j+1,
                                    interp_barriers);
  }

  // Now interpolate in energy.
  // But if we're outside of the range of the table, just use the value
  // at the end (don't extrapolate).
  float par[3];
  for (int i=0; i < 3; i++) {
    if (end-beg > 1)
      par[i] = interpolate (partab, energy, energy_degree, i+1);
    else
      par[i] = partab[beg][i+1];
  }

  // Calculate the fit function.
  // Don't allow b to go all the way to zero; we get a division
  // by zero there.
  double b = std::max ((double)par[1], 1e-5);
  double atanb = std::atan(b);
  double sq = std::sqrt (b/atanb - 1);
  double den = (sq/b*atanb - std::atan(sq));
  float offs = par[0]* ((- std::atan (b*u) + u*atanb) / den +
                        par[2]*(1-std::abs(u)));

  // Apply the offset correction to the cluster.
  if (eta < 0)
    offs = -offs;
  cluster->setEta (samp, eta + offs);
}
