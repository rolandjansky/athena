// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonTruthTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Helpers to categorize photon TruthParticle's.
 *
 * Adapted from code in PhotonAnalysisUtils.
 */


#ifndef EGAMMAD3PDANALYSIS_PHOTONTRUTHTOOL_H
#define EGAMMAD3PDANALYSIS_PHOTONTRUTHTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEgamma/Photon.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include <string>


namespace D3PD {


/// Interface definition.
static const InterfaceID IID_PhotonTruthTool ("D3PD::PhotonTruthTool", 1, 0);


class PhotonTruthTool
  : public AthAlgTool
{
public:
  /// Gaudi interface definition.
  static const InterfaceID& interfaceID() { return IID_PhotonTruthTool; }

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PhotonTruthTool (const std::string& type,
                   const std::string& name,
                   const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c queryInterface method.
  virtual StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvIf );


  /**
   * @brief Go from a photon to a matching @c TruthParticle.
   * @param g The input photon.
   * @param cache Truth classifier association cache.
   * @return The matching @c TruthParticle, or null.
   */
  const xAOD::TruthParticle*
  toTruthParticle (const xAOD::Photon& g) const;


  /**
   * @brief Check a truth particle for a conversion.
   * @param truePart The  particle to check.
   * @param[out] RconvMC Radius of the conversion.
   * @param[out] ZconvMC Z of the conversion.
   * @return True if this is a conversion; otherwise false.
   */
  bool getMCConv (const xAOD::TruthParticle* truePart,
                  float& RconvMC,
                  float& ZconvMC) const;


  /// Test for a prompt photon.
  bool isPromptPhotonMC (const xAOD::TruthParticle* truePart)  const;


  /// Test for a prompt particle.
  bool isPromptParticleMC (const xAOD::TruthParticle* truePart)  const;


  /// Test for a brem.
  bool isQuarkBremMC (const xAOD::TruthParticle* truePart)  const;


private:
  /// Test for a final-state photon.
  bool isFinalStatePhotonMC (const xAOD::TruthParticle* truePart) const;

  
  /// Test for a final-state particle.
  bool isFinalState(const xAOD::TruthParticle* truePart)  const;


  /// Get the (first) mother particle of @a p.
  const xAOD::TruthParticle*
  getMother(const xAOD::TruthParticle* p) const;


  /// Get the mother vertex for @a p.
  const xAOD::TruthVertex*
  getMotherVert(const xAOD::TruthParticle* p) const;


  /// Return list of mother particles of @a p.
  std::vector<const xAOD::TruthParticle*>
  getMothers(const xAOD::TruthParticle* p) const;


  /// Property: classifier tool.
  ToolHandle<IMCTruthClassifier> m_classifier;

  /// Property: Conversion vertex z cut.
  float m_zTruthConv;

  /// Property: Conversion vertex r cut.
  float m_rTruthConv;

  /// Property.
  bool m_useG4Particles;
};


} // namespace D3PD


#endif // EGAMMAD3PDANALYSIS_PHOTONTRUTHTOOL_H
