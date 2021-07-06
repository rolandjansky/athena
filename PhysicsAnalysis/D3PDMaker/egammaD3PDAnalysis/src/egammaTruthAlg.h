// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file egammaD3PDAnalysis/src/egammaTruthAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Select egtruth particles.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMATRUTHALG_H
#define EGAMMAD3PDANALYSIS_EGAMMATRUTHALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>


namespace D3PD {


/**
 * @brief Select egtruth particles.
 */
class egammaTruthAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaTruthAlg (const std::string& name,
                  ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize() override;


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute() override;


private:
  /**
   * @brief Test to see if we accept a particle.
   * @param tp The particle to test.
   * @param cont The container of particles.
   * @param iso[out] The isolation energy for this particle.
   */
  bool isAccepted (const xAOD::TruthParticle& tp,
                   const xAOD::TruthParticleContainer& cont,
                   float& iso);


  /**
   * @brief Compute isolation around a particle.
   * @param tp The particle for which we want isolation.
   * @param cont The container of particles.
   */
  float computeIso (const xAOD::TruthParticle& tp,
                    const xAOD::TruthParticleContainer& cont);


  /**
   * @brief Find the impact of a particle in the calorimeter.
   * @param p The particle to analyze.
   * @param etaCalo[out] Eta of the particle's impact with the calorimeter.
   * @param phiCalo[out] Phi of the particle's impact with the calorimeter.
   */
  StatusCode findImpact (const xAOD::TruthParticle& tp,
                         float& etaCalo,
                         float& phiCalo);


  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Property: Name of the input container.
  std::string m_inputKey;

  /// Property: Name of the output container.
  std::string m_outputKey;

  /// Property: Minimum pt for electrons.
  float m_electronPtMin;

  /// Property: Minimum pt for photons.
  float m_photonPtMin;

  /// Property: Maximum eta.
  float m_etaMax;

  /// Property: Isolation cone width.
  float m_isoCone;

  /// Property: Maximum isolation cone energy allowed to keep a photon.
  float m_photonEtIsoMax;

  /// Property: Extrapolation tool to calorimeter.
  ToolHandle<Trk::IParticleCaloExtensionTool> m_exten
  { this, "ParticleCaloExtensionTool", "", "Extrapolator to calorimeter." };
};


} // namespace D3PD


#endif // not EGAMMAD3PDANALYSIS_EGAMMATRUTHALG_H
