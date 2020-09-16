/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOCLUSTERMOMENTSMAKER_H
#define CALOCLUSTERMOMENTSMAKER_H
/**
 * @class CaloClusterMomentsMaker
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 28-February-2005
 * @brief Calculate moments for CaloCluster objects
 *
 * This is a CaloClusterCollectionProcessor which can be plugged into a
 * CaloClusterMaker for calculating moments for each CaloCluster.
 * Typically a cluster moment of a certain degree \f$n\f$ over a
 * variable \f$x\f$ defined for each CaloCell member of a CaloCluster
 * is defined as:
 *
 * \f[ \langle x^n\rangle = \frac{1}{E_{\rm norm}} \times
 * \!\!\!\sum\limits_{\{{\rm cell}|E_{\rm cell}>0\}}{\!\!\!E_{\rm
 * cell}\, x^n},\f] with \f[ E_{\rm norm} = \!\!\!\sum\limits_{\{{\rm
 * cell}|E_{\rm cell}>0\}}{\!\!\!E_{\rm cell}}. \f]
 *
 * Note that only cells with positive energy are used in this definition.
 * Common variables to calculate first and second moments of are
 * \f$\phi\f$, \f$\eta\f$, and radial and longitudinal distances from
 * the shower axis and the shower center, respectively.  */

#include "GaudiKernel/ToolHandle.h"

class CaloDetDescrManager; 
class CaloDetDescrElement;
class CaloCell_ID;

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloInterface/ILArHVFraction.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>
#include <vector>

class CaloClusterMomentsMaker: public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:    
  CaloClusterMomentsMaker(const std::string& type, const std::string& name,
			  const IInterface* parent);

  using CaloClusterCollectionProcessor::execute;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* theClusColl) const override final;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
 private: 
  
  /** 
   * @brief vector holding the input list of names of moments to
   * calculate.
   *
   * This is the list of desired names of moments given in the
   * jobOptions.*/
  std::vector<std::string>  m_momentsNames; 

  /** 
   * @brief set of moments which will be calculated.
   *
   * This set will hold each valid enum indexed if its name
   * was found on the input list (m_momentsNames) and in the list of
   * valid moment names (m_validNames). */
  std::vector<xAOD::CaloCluster::MomentType> m_validMoments;

  const CaloCell_ID* m_calo_id;

  /** @brief the maximal allowed deviation from the
   * IP-to-ClusterCenter-axis. */
  double m_maxAxisAngle; 

  /**
   * @brief the minimal \f$r\f$ in the definition of the Lateral moment
   *
   * This defines the minimal distance the two leading cells might
   * have before this value is used instead of their real distance in
   * the normalization of the LATERAL moment. */
  double m_minRLateral;  

  /**
   * @brief the minimal \f$\lambda\f$ in the definition of the
   * Longitudinal moment
   *
   * This defines the minimal distance along the shower axis from the
   * cluster center the two leading cells might have before this value
   * is used instead of their real distance in the normalization of
   * the LONGITUDINAL moment. */
  double m_minLLongitudinal;

  /**
   * @brief the minimal cell quality in the LAr for declaring a cell bad
   *
   * This defines the minimal quality (large values mean worse shape)
   * a cell needs to exceed in order to be considered as not
   * compatible with a normal ionization signal. */
  double m_minBadLArQuality;

  /// Set to true if significance moments are need
  bool m_calculateSignificance;

  /// Set to true if cluster isolation is to be calculated
  bool m_calculateIsolation;

  /// Set to true to calculate E and N of cells affected by LAr HV corrections
  bool m_calculateLArHVFraction;

  /**
   * @brief if set to true use 2-gaussian noise description for
   * TileCal  */
  bool m_twoGaussianNoise;

  ToolHandle<CaloDepthTool> m_caloDepthTool;

  /** @brief Key of the CaloNoise Conditions data object. Typical values 
      are '"electronicNoise', 'pileupNoise', or '"totalNoise' (default) */
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};

  ToolHandle<ILArHVFraction> m_larHVFraction;

  /// Not used anymore (with xAOD), but required when configured from COOL.
  std::string m_momentsNamesAOD;
  
  /**
   * @brief if set to true use abs E value of cells to calculate 
   * cluster moments */
   bool m_absOpt;
};

#endif // CALOCLUSTERMOMENTSMAKER_H
