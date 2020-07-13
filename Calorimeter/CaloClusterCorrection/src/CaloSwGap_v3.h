// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloSwGap_v3.h
 * Sandrine should document this
 */


#ifndef CALOCLUSTERCORRECTION_CALOSWGAP_V3_H
#define CALOCLUSTERCORRECTION_CALOSWGAP_V3_H


#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloConditions/Array.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
#include "StoreGate/ReadHandleKey.h"


class CaloCellContainer;


/**
 * @class CaloSwGap_v3
 * @brief EM calorimeter gap corrections.
 *
 * This tool attempts to correct for the energy lost in dead material
 * in the gap between the barrel and end cryostats.  In addition to
 * the energy in the EM calorimeter, we also use energy deposited in
 * scintillators between the cryostats.  The correction is of the
 * form @f[
 *
 *    E' = A(E_c + \alpha E_s),
 *
 * @f] where @f$E_c@f$ is the measured energy in the EM calorimeter,
 * and @f$E_s@f$ is the measured energy in the scintillator.  The
 * coefficients @f$A@f$ and @f$\alpha@f$ are @f$\eta@f$-dependent.
 * The total scale @f$A@f$ is determined by fitting a polynomial
 * to the @f$E_{\rm meas}/E_{\rm true}@f$ curve outside the gap
 * region, and interpolating over the gap.  The weight @f$\alpha@f$ is
 * found by requiring that the RMS of @f$E'@f$ be a minimum.
 *
 * The resulting weights do not vary a lot with energy in the range
 * studied (50 to 200 GeV).  Thus, we combine together all the energies
 * when determining the correction; the resulting correction is then
 * energy-independent.  The residual energy dependence will be folded
 * into subsequent corrections.
 *
 */
class CaloSwGap_v3
  : public CaloClusterCorrectionCommon,
    public ISetCaloCellContainerName
{
public:
  /// Inherit constructor.
  using CaloClusterCorrectionCommon::CaloClusterCorrectionCommon;


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Virtual function for the correction-specific code.
   * @param myctx   ToolWithConstants context.
   * @param ctx     The event context.
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
  virtual void makeTheCorrection (const Context& myctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const override;



  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name) override;

  virtual bool isGoodPhi(float eta, float phi) const;

private:
  /// Calibration constants: The range over which this correction is defined.
  Constant<float> m_etamin_crack
  { this, "etamin_crack", "Lower end of range over which this correction is defined." };
  Constant<float> m_etamax_crack
  { this, "etamax_crack", "Upper end of range over which this correction is defined." };

  /// Calibration constant: The interpolation degree.
  Constant<int> m_degree
  { this, "degree", "Interpolation degree." };

  /// Calibration constant: The tabulated array of correction weights,
  /// A and alpha.
  Constant<CxxUtils::Array<2> > m_correctionGoodPhi
  { this, "correctionGoodPhi", "" };
  Constant<CxxUtils::Array<2> > m_correctionBadPhi
  { this, "correctionBadPhi", "" };

  /// Calibration constant: If true, tabulated values are in terms of
  /// raw (local) eta.
  Constant<bool> m_use_raw_eta
  { this, "use_raw_eta", "If true, tabulated values are in terms of raw (local) eta." };
  Constant<bool> m_use_raw_eta_boundaries
  { this, "use_raw_eta_boundaries", "" };

  /// Property: The name of the container in which to look to find tile cells.
  SG::ReadHandleKey<CaloCellContainer> m_cells_name
  { this, "cells_name", "AllCalo", "The name of the container in which to look to find tile cells." };
};


#endif // not CALOCLUSTERCORRECTION_CALOSWGAP_V3_H
