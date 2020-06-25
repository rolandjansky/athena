// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* @file  CaloClusterCorrectionCommon.h
 * @author scott snyder <snyder@bnl.gov>
 * @date December, 2004
 * @brief Code common to cluster corrections.
 */

#ifndef CALOCLUSTERCORRECTIONCOMMON_H
#define CALOCLUSTERCORRECTIONCOMMON_H


#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
class CaloDetDescrManager;
class CaloDetDescrElement;


namespace CaloClusterCorr {
class DDHelper;
}


/**
 * @class CaloClusterCorrectionCommon
 * @brief Code common to cluster corrections.
 *
 * This class is the base for many of the calorimeter cluster corrections.
 * It factors out code common between them.
 *
 * This class holds one constant, @c m_region, saying which
 * region of the calorimeter this correction is for.   The
 * @c makeCorrection routine here will determine if the cluster is
 * in the desired region and, if so, call the virtual method
 * @c makeTheCorrection.
 */
class CaloClusterCorrectionCommon
  : public CaloClusterCorrection
{
public:
  /// Inherit constructor.
  //using CaloClusterCorrection::CaloClusterCorrection;
  CaloClusterCorrectionCommon (const std::string& type,
                               const std::string& name,
                               const IInterface* parent);


  /// Destructor.
  virtual ~CaloClusterCorrectionCommon();


  /**
   * @brief Perform the correction.  Called by the tool
   * @param myctx   ToolWithConstants context.
   * @param cluster The cluster to correct.
   *                It is updated in place.
   */
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;


  /**
   * @brief Virtual function for the correction-specific code.
   * @param myctx   ToolWithConstants context.
   * @param cluster The cluster to correct.
   *                It is updated in place.
   * @param elt     The detector description element corresponding
   *                to the cluster location.
   * @param eta     The @f$\eta@f$ coordinate of the cluster.
   * @param adj_eta The @f$\eta@f$ adjusted for
   *                any shift between the actual and nominal coordinates.
   *                (This is shifted back to the nominal coordinate system.)
   * @param phi     The @f$\phi@f$ coordinate of the cluster.
   * @param adj_phi The @f$\phi@f$ adjusted for
   *                any shift between the actual and nominal coordinates.
   *                (This is shifted back to the nominal coordinate system.)
   * @param samp    If we're looking at a particular sampling,
   *                this is the calorimeter sampling we're examining.
   *                It is a sampling code as defined by
   *                @c CaloSampling::CaloSample; i.e., it has both
   *                the calorimeter region and sampling encoded.
   *                Otherwise, this is Unknown.
   */
  virtual void makeTheCorrection (const Context& myctx,
                                  xAOD::CaloCluster* cluster,
                                  const CaloDetDescrElement* elt,
                                  float eta,
                                  float adj_eta,
                                  float phi,
                                  float adj_phi,
                                  CaloSampling::CaloSample samp) const = 0;


  /// Region codes for @c m_region below.
  /// This is used to decide how to report the position of the cluster.
  /// Note: This numbering is also used in the job options files.
  enum {
    // Barrel, sampling 1.
    EMB1 = 0,

    // Barrel, sampling 2.
    EMB2 = 1,

    // Endcap, sampling 1.
    EME1 = 2,

    // Endcap, sampling 2.
    EME2 = 3,

    // Average of barrel and endcap in sampling 2.
    COMBINED2 = 4,

    // Overall cluster position.
    CLUSTER = 5,

    // Number of different region codes.
    NREGIONS = 6
  };


  /// Helper, used to calculate the values of the energy
  /// interpolation table.
  class TableBuilder
  {
  public:
    virtual ~TableBuilder() {}

    /// Calculate the correction for tabulated energy @c energy_ndx.
    /// If successful, set @c good to true; otherwise, set it to false.
    virtual float calculate (int energy_ndx, bool& good) const = 0;
  };


  /// Many of the corrections use the same method for doing the final
  /// interpolation in energy.  We factor out this common code here.
  /// @c builder is used to construct the interpolation table;
  /// @c energy is the energy value for the interpolation.
  /// @c energies is the list of energies at which we have
  /// tabulated values, and @c energy_degree is the degree of the
  /// polynomial interpolation in energy.
  /// 
  float energy_interpolation (float energy,
                              const TableBuilder& builder,
                              const CaloRec::Array<1>& energies,
                              int energy_degree) const;



private:
  /// Retrieve the detector description helper,
  /// creating it if needed.
  const CaloClusterCorr::DDHelper& ddhelper() const;

  /// Calibration constant: The calorimeter region for which this correction
  /// is intended.  This should be one of the constants above.
  /// This affects the meaning of the @c eta and @c phi arguments
  /// passed to @c makeTheCorrection, as well as the @c samp argument.
  Constant<int> m_region
  { this, "region", "Calorimeter region" };

  /// Helper for detector description lookup.
  CxxUtils::CachedUniquePtr<const CaloClusterCorr::DDHelper> m_ddhelper;
};


#endif // not CALOCLUSTERCORRECTIONCOMMON_H
