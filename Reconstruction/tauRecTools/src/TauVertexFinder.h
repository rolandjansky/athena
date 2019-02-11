/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUVERTEXFINDER_H
#define TAUREC_TAUVERTEXFINDER_H

#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandle.h"
#include "xAODTracking/VertexContainer.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"



/////////////////////////////////////////////////////////////////////////////

/** 
 * @brief Associate a vertex (origin) to the tau candidate.
 * 
 *  The vertex has to be consistent with the origin of tau jet seed.
 *  The vertex is determined in the following order: TJVA -> first PV in vertex container -> 0
 *  Attention: all downstream tau variables will be calculated to the vertex set here!
 * 
 * @author KG Tan <Kong.Guan.Tan@cern.ch>
 * @author Felix Friedrich <Felix.Friedrich@cern.ch>
 */

class TauVertexFinder : public TauRecToolBase {
public:
  //-------------------------------------------------------------
  //! Constructor and Destructor
  //-------------------------------------------------------------
  TauVertexFinder(const std::string& name);
  ASG_TOOL_CLASS2(TauVertexFinder, TauRecToolBase, ITauToolBase);
  ~TauVertexFinder();

  //-------------------------------------------------------------
  //! Algorithm functions
  //-------------------------------------------------------------
  virtual StatusCode initialize();
  virtual StatusCode eventInitialize();
  virtual StatusCode execute(xAOD::TauJet& pTau);
  virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
  virtual StatusCode eventFinalize();
  virtual StatusCode finalize();

  virtual void cleanup(xAOD::TauJet* ) { }
  virtual void print() const { }


private:
  ElementLink<xAOD::VertexContainer>
  getPV_TJVA(const xAOD::TauJet& tauJet,
             const xAOD::VertexContainer& vertices,
             float& maxJVF);

  float getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const;
  // for online ATR-15665
  float getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks) const;      
  
private:
  ToolHandle< InDet::IInDetTrackSelectionTool > m_TrackSelectionToolForTJVA;

  // for online
  std::vector<const xAOD::Vertex*> m_matchedVertexOnline;
  //-------------------------------------------------------------
  //! Configureables
  //-------------------------------------------------------------
  bool m_useTJVA;
  std::string m_assocTracksName;
  // for online ATR-15665
  float m_transDistMax;
  float m_longDistMax;
  float m_maxZ0SinTheta;
  
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "InDetTrackParticles", "input vertex container key"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackPartInputContainer{this,"Key_trackPartInputContainer", "InDetTrackParticles", "input track particle container key"};   
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_jetTrackVtxAssoc{this, "Key_JetTrackVtxAssoc_forTaus", "JetTrackVtxAssoc_forTaus", "input TVA for taus"};

};

#endif // not TAUREC_TAUVERTEXFINDER_H
