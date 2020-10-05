/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  StatusCode initialize() override;
  StatusCode executeVertexFinder(xAOD::TauJet& pTau,
                                const xAOD::VertexContainer* vertexContainer = nullptr,
                                const xAOD::TrackParticleContainer* trackContainer = nullptr) const override;

private:
  ElementLink<xAOD::VertexContainer>
  getPV_TJVA(const xAOD::TauJet& tauJet,
             const xAOD::VertexContainer& vertices,
             const xAOD::TrackParticleContainer* trackContainer,
             float& maxJVF) const;

  float getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const;
  // for online ATR-15665
  float getJetVertexFraction(const xAOD::Vertex* vertex,
                             const std::vector<const xAOD::TrackParticle*>& tracks,
                             const std::vector<const xAOD::Vertex*>& matchedVertexOnline) const;
  
private:
  
  Gaudi::Property<bool> m_useTJVA {this, "UseTJVA", true};
  Gaudi::Property<std::string> m_assocTracksName {this, "AssociatedTracks", ""};
  Gaudi::Property<float> m_transDistMax {this, "OnlineMaxTransverseDistance", 10e6};
  Gaudi::Property<float> m_longDistMax {this, "OnlineMaxLongitudinalDistance", 10e6};
  Gaudi::Property<float> m_maxZ0SinTheta {this, "OnlineMaxZ0SinTheta", 10e6};
  
  ToolHandle< InDet::IInDetTrackSelectionTool > m_TrackSelectionToolForTJVA {this, "InDetTrackSelectionToolForTJVA", ""};
  
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackPartInputContainer{this,"Key_trackPartInputContainer", "InDetTrackParticles", "input track particle container key"};   
  SG::ReadHandleKey<jet::TrackVertexAssociation> m_jetTrackVtxAssoc{this, "Key_JetTrackVtxAssoc_forTaus", "JetTrackVtxAssoc_forTaus", "input TVA for taus"};
};

#endif // not TAUREC_TAUVERTEXFINDER_H
