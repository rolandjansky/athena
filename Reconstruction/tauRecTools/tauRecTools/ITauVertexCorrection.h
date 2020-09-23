/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUVERTEXCORRECTION_H
#define ITAUVERTEXCORRECTION_H

#include "AsgTools/IAsgTool.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTau/TauJet.h"


/**
 * @brief Tool to perform the vertex correction for the tau candidate
 */

class ITauVertexCorrection : virtual public asg::IAsgTool {
 
public:

  ASG_TOOL_INTERFACE(ITauVertexCorrection) 

  virtual ~ITauVertexCorrection() = default;

  virtual StatusCode initialize() = 0;
  
  /** Get the jet vertex */
  virtual const xAOD::Vertex* getJetVertex(const xAOD::Jet& jet) const = 0;

  /** Get the tau axis */
  virtual TLorentzVector getTauAxis(const xAOD::TauJet& tau) const = 0;
  
  /**@brief Get the vertex corrected four momentum, the position of 
   *        the tau vertex relative to the jet vertex is used
   * @param cluster input cluster
   * @param vertex the vertex of the tau candidate
   * @param relativeVertex the vertex of the jet seed
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::CaloCluster& cluster, 
                                      const xAOD::Vertex* vertex,
                                      const xAOD::Vertex* relativeVertex = nullptr) const = 0;
  
  /**@brief Get the vertex corrected four momentum, the position of 
   *        the tau vertex relative to the jet vertex is used
   * @param pfo input PFO
   * @param vertex the vertex of the tau candidate
   * @param relativeVertex the vertex of the jet seed
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::PFO& pfo, 
                                      const xAOD::Vertex* vertex, 
                                      const xAOD::Vertex* relativeVertex = nullptr) const = 0; 

  /**@brief Get the vertex corrected four momentum, the position of 
   *        the tau vertex relative to the jet vertex is used
   * @param constituent input constituent
   * @param vertex the vertex of the tau candiate
   * @param relativeVertex the vertex of the jet seed
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::JetConstituent& constituent, 
                                      const xAOD::Vertex* vertex, 
                                      const xAOD::Vertex* relativeVertex = nullptr) const = 0; 

private:

  /**@brief Get the vertex corrected four momentum
   * @param cluster input cluster
   * @param position position of the vertex
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::CaloCluster& cluster, 
                                      const Amg::Vector3D& position) const = 0;
  
  /**@brief Get the vertex corrected four momentum
   * @param pfo input PFO
   * @param position position of the vertex
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::PFO& pfo, 
                                      const Amg::Vector3D& position) const = 0;
};

#endif // ITAUVERTEXCORRECTION_H
