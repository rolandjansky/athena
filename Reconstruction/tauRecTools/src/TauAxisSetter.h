/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUAXISSETTER_H
#define TAURECTOOLS_TAUAXISSETTER_H

#include "tauRecTools/TauRecToolBase.h"

#include "AsgTools/ToolHandle.h"

/**
 * @brief Set Tau "Detector Axis" and "Intermediate Axis". 
 * 
 *  Note that both axes starts from the barycenter of the cluster associated to the jet seed. 
 *  Then only the 4-vectors of clusters in a cone of dR around these barycenter are summed up, forming the detector axis.
 *  For the "Intermediate Axis", the clusters are corrected wrt tau vertex in this step (barycenter remains the same).
 *  Using this procedure, the axes are different from the original jet seed axis.
 * 
 * @author Margar Simonyan
 * @author Felix Friedrich
 *                                                                              
 */

class TauAxisSetter : public TauRecToolBase {

public:
  
  ASG_TOOL_CLASS2(TauAxisSetter, TauRecToolBase, ITauToolBase)
  
  /** @brief Constructor */ 
  TauAxisSetter(const std::string& name);

  /** @brief Destructor */
  virtual ~TauAxisSetter() = default;

  /** @brief Execution of this tool */ 
  virtual StatusCode execute(xAOD::TauJet& tau) const override;

private:
  
  /**@brief Get the vertex corrected four momentum */
  TLorentzVector getVertexCorrectedP4(const xAOD::JetConstituent& constituent, 
                                      const Amg::Vector3D& position) const; 
  
  /**@brief Get the vertex corrected four momentum */
  TLorentzVector getVertexCorrectedP4(const xAOD::PFO& pfo, 
                                      const Amg::Vector3D& position) const; 

  Gaudi::Property<double> m_clusterCone {this, "ClusterCone", 0.2, "cone of tau candidate"};
  Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of tau vertex correction"};
};

#endif // TAURECTOOLS_TAUAXISSETTER_H
