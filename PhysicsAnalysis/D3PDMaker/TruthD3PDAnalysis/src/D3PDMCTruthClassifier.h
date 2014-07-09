// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDAnalysis/src/D3PDMCTruthClassifier.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2012
 * @brief Work around MCTruthClassifier brain-damage.
 *
 * This avoids having to specify the container names during configuration.
 */


#include "MCTruthClassifier/MCTruthClassifier.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


#ifndef D3PDMCTRUTHCLASSIFIER_H
#define D3PDMCTRUTHCLASSIFIER_H


namespace D3PD {


/**
 * @brief Work around MCTruthClassifier brain-damage.
 *
 * This avoids having to specify the container names during configuration.
 *
 * MCTruthClassifier requires specifying, at configuration time,
 * the names of the TrackParticleContainer and TrackParticleTruthCollection
 * objects to use for the mapping.  This is painful in for electrons
 * in the case where one may have several different track containers.
 *
 * What we do to make this easier is to search through SG to find
 * a truth collection that points at the TrackParticleContainer
 * that contains the tracks for the electron we're looking at.
 */
class D3PDMCTruthClassifier
  : public MCTruthClassifier
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  D3PDMCTruthClassifier(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  using MCTruthClassifier::particleTruthClassifier;


  /**
   * @brief Run the classifier for an electron.
   * @param el The input electron.
   */
  virtual std::pair<MCTruthPartClassifier::ParticleType,
                    MCTruthPartClassifier::ParticleOrigin>
  particleTruthClassifier (const xAOD::Electron* el);


  /**
   * @brief Run the classifier for a photon.
   * @param el The input photon.
   */
  virtual std::pair<MCTruthPartClassifier::ParticleType,
                    MCTruthPartClassifier::ParticleOrigin>
  particleTruthClassifier (const xAOD::Photon* el);


  /**
   * @brief Run the classifier for a muon.
   * @param el The input photon.
   */
  virtual std::pair<MCTruthPartClassifier::ParticleType,
                    MCTruthPartClassifier::ParticleOrigin>
  particleTruthClassifier (const xAOD::Muon* mu);


private:
  /**
   * @brief Try to find a TrackParticleTruthCollection pointing at TP_NAME.
   * @param tp_name The name of a TrackParticleContainer.
   * @param tptruth_name[out] The name of a TrackParticleTruthCollection
   *                          pointing at it.
   * @returns True if we found a good truth collection.
   */
  bool get_tptruth_name (const std::string& tp_name,
                         std::string& tptruth_name);


  /**
   * @brief Helper to retrieve the value of a Gaudi property.
   * @param pname The name of the property.
   * param value[out] The value of the property.
   */
  void getProperty1 (const std::string& pname, std::string& value);


  /**
   * @brief Helper to set the value of a Gaudi property.
   * @param pname The name of the property.
   * param value The new value of the property.
   */
  void setProperty1 (const std::string& pname, const std::string& value);


  /// The StoreGate service.
  ServiceHandle<StoreGateSvc> m_sg;
};


} // namespace D3PD



#endif // not D3PDMCTRUTHCLASSIFIER_H
