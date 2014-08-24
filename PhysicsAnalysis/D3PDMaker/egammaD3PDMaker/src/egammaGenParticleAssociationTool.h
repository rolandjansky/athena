// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaGenParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an electron to a matched GenParticle via
 *        classification tool.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMAGENPARTICLEASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_EGAMMAGENPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Associate from an electron/photon to a matched GenParticle via
 *        classification tool.
 */
class egammaGenParticleAssociationTool
  : public SingleAssociationTool<xAOD::Egamma, xAOD::TruthParticle>
{
public:
  typedef SingleAssociationTool<xAOD::Egamma, xAOD::TruthParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaGenParticleAssociationTool (const std::string& type,
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
  virtual const xAOD::TruthParticle* get (const xAOD::Egamma& p);


private:
  /// Property: classifier tool.
  ToolHandle<IMCTruthClassifier> m_classifier;

  /// Property: If not empty, the variable name to use for DR.
  std::string m_drvar;

  /// Variable: DR between reco and truth.
  float* m_dr;
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_EGAMMAGENPARTICLEASSOCIATIONTOOL_H
