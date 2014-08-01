// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/MuonGenParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2010
 * @brief Associate from a muon to a matched GenParticle via
 *        classification tool.
 */


#ifndef MUOND3PDMAKER_MUONGENPARTICLEASSOCIATIONTOOL_H
#define MUOND3PDMAKER_MUONGENPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Associate from an electron/photon to a matched GenParticle via
 *        classification tool.
 */
class MuonGenParticleAssociationTool
  : public SingleAssociationTool<xAOD::Muon, xAOD::TruthParticle>
{
public:
  typedef SingleAssociationTool<xAOD::Muon, xAOD::TruthParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonGenParticleAssociationTool (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);


  /// Standard Gaudi initialize method.
  StatusCode initialize();


  /// Create any needed tuple variables.
  StatusCode book ();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::TruthParticle* get (const xAOD::Muon& p);


private:
  /// Property: classifier tool.
  ToolHandle<IMCTruthClassifier> m_classifier;

  /// Property: If not empty, the variable name to use for DR.
  std::string m_drvar;

  /// Variable: DR between reco and truth.
  float* m_dr;
};


} // namespace D3PD



#endif // not MUOND3PDMAKER_MUONGENPARTICLEASSOCIATIONTOOL_H
