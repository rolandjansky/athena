/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUVERTEXCORRECTION_H
#define TAUVERTEXCORRECTION_H

#include "tauRecTools/ITauVertexCorrection.h"

#include "AsgTools/AsgTool.h"
#include "AsgDataHandles/ReadHandleKey.h"

/**
 * @brief Tool to perform the vertex correction for the tau candidate
 * @author Xiaozhong Huang <xiaozhong.huang@cern.ch>  
 */

class TauVertexCorrection : public asg::AsgTool, virtual public ITauVertexCorrection {
 
public:

  ASG_TOOL_INTERFACE(TauVertexCorrection)
  ASG_TOOL_CLASS1(TauVertexCorrection, ITauVertexCorrection)

  TauVertexCorrection(const std::string& name);
  virtual ~TauVertexCorrection() = default;

  virtual StatusCode initialize() override;
  
  /** Get the jet vertex */
  virtual const xAOD::Vertex* getJetVertex(const xAOD::Jet& jet) const override;
  
  /** Get the tau axis */
  virtual TLorentzVector getTauAxis(const xAOD::TauJet& tau) const override;
  
  /**@brief Get the vertex corrected four momentum, the position of 
   *        the tau vertex relative to the jet vertex is used
   * @param cluster input cluster
   * @param tauVertex the vertex of the tau candidate
   * @param jetVertex the vertex of the jet seed
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::CaloCluster& cluster, 
                                      const xAOD::Vertex* tauVertex,
                                      const xAOD::Vertex* jetVertex = nullptr) const override;
  
  /**@brief Get the vertex corrected four momentum, the position of 
   *        the tau vertex relative to the jet vertex is used
   * @param pfo input PFO
   * @param tauVertex the vertex of the tau candidate
   * @param jetVertex the vertex of the jet seed
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::PFO& pfo, 
                                      const xAOD::Vertex* tauVertex, 
                                      const xAOD::Vertex* jetVertex = nullptr) const override; 

  /**@brief Get the vertex corrected four momentum, the position of 
   *        the tau vertex relative to the jet vertex is used
   * @param constituent input constituent
   * @param tauVertex the vertex of the tau candiate
   * @param jetVertex the vertex of the jet seed
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::JetConstituent& constituent, 
                                      const xAOD::Vertex* tauVertex, 
                                      const xAOD::Vertex* jetVertex = nullptr) const override; 

private:
  
  /**@brief Get the vertex corrected four momentum
   * @param cluster input cluster
   * @param position position of the vertex
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::CaloCluster& cluster, 
                                      const Amg::Vector3D& position) const override;
  
  /**@brief Get the vertex corrected four momentum
   * @param pfo input PFO
   * @param position position of the vertex
   */
  virtual TLorentzVector getVertexCorrectedP4(const xAOD::PFO& pfo, 
                                      const Amg::Vector3D& position) const override;
  
  /// Name of the seed jet
  std::string m_seedJet;

  /// Switch of the tau vertex correction
  bool m_doVertexCorrection; 

  /// Switch of the jet vertex correction
  bool m_doJetVertexCorrection;

  /// Whehter the seed jet is PFO 
  bool m_isPFO = false; //!
  
  /// Calibration state of cluster
  xAOD::CaloCluster::State m_clusterState; //!
  
};

#endif // ITAUVERTEXCORRECTION_H
