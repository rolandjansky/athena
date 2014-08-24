// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/ElectronJetDRAssociator.h
 * @author Clemencia Mora <Clemencia.Mora.Herrera@cern.ch>
 * @date May, 2010
 * @brief For every electron find jets in a cone and store lifetime, signed IP,
 *        and PTRel of relation.
 */


#ifndef EGAMMAD3PDMAKER_ELECTRONJETDRASSOCIATOR_H
#define EGAMMAD3PDMAKER_ELECTRONJETDRASSOCIATOR_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODJet/Jet.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>



namespace Trk  { class VxCandidate; class ITrackToVertexIPEstimator; }


namespace D3PD {


class ICollectionGetterRegistryTool;
class ICollectionGetterTool;


/**
 * @brief For every electron find jets in a cone and store lifetime, signed IP,
 *        and PTRel of relation.
 */
class ElectronJetDRAssociator
  : public MultiAssociationTool<xAOD::Electron,xAOD::Jet>
{
public:
  typedef MultiAssociationTool<xAOD::Electron,xAOD::Jet> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ElectronJetDRAssociator (const std::string& type,
                           const std::string& name, 
                           const IInterface* parent);
  

  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize() override;


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c resetUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * (to @c getUntyped)
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti) override;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const xAOD::Electron& eg) override;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const xAOD::Jet* next() override;


  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book() override;


private:
  /**
   * @brief Find the pT of the jet with electron removed.
   * @param eg The egamma object to analyze.
   * @param j The jet under consideration.
   * @reducedPt[out] Jet Pt calculated after subtraction of electron momentum.
   */
  StatusCode findReducedPt (const xAOD::Electron* eg,
			    const xAOD::Jet* j,
			    float& reducedPt);

  /**
   * @brief Find the signed impact parameter and PTRel of EG's track
   *        relative to the beamspot.
   * @param eg The egamma object to analyze. 
   * @param signedIP[out] Transverse impact parameter wrt the beamspot.
   * @param PTRel[out] Longitudinal impact parameter wrt the beamspot.
   */
  StatusCode findSignedIPAndPTRelEleJetPair (const xAOD::Electron* eg,
                                             const xAOD::Jet* j0,
                                             float& signedIP, float& PTRel);


  /// Property: Target label for the getter for the jets to which to associate.
  std::string m_targetLabel;

  /// Property: Vertex container name.
  std::string m_vertexContainerName;

  /// Property: DR cut between electrons and jets.
  double m_drcut;

  /// Property: Tool for the estimation of the IPs to the vertex.
  ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;

  /// Property: The ICollectionGetterRegistryTool instance.
  ToolHandle<ICollectionGetterRegistryTool> m_registry;


  /// Getter defining the collection within which to index.
  ICollectionGetterTool* m_target;

  // Input electrons; source of the association.
  const xAOD::Electron* m_eg;

  /// Variable: DR between electron and jet.
  float* m_dr;

  /// Variable: Signed impact parameter between electron and jet.
  float* m_signedIP;

  /// Variable: Relative Pt between electron and jet.
  float* m_ptrel;

  /// Variable: Number of jet constituents.
  unsigned int* m_jetSize;

  /// Variable: Jet pT with electron removed.
  float* m_reducedPt;
};


} // namespace D3PD


#endif // EGAMMAD3PDMAKER_EGAMMAJETDRASSOCIATOR_H
