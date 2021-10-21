/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetSecVertexTruthMatchTool_h
#define InDetSecVertexTruthMatchTool_h

// Framework include(s):
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTruth/TruthParticleFwd.h"
#include "xAODTruth/TruthVertexFwd.h"
#include "InDetSecVertexValidation/IInDetSecVertexTruthMatchTool.h"

// standard includes
#include<vector>

/** Class for vertex truth matching.
 * Match reconstructed vertices to truth level interactions vertices
 * through the chain: track -> particle -> genEvent -> genVertex
 * Categorize reconstructed vertices depending on their composition.
 */


class InDetSecVertexTruthMatchTool : public virtual IInDetSecVertexTruthMatchTool,
                                  public asg::AsgTool {

  ASG_TOOL_CLASS( InDetSecVertexTruthMatchTool, IInDetSecVertexTruthMatchTool )

 public:

  InDetSecVertexTruthMatchTool( const std::string & name );

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override;

  //take const collection of vertices, match them, and decorate with matching info
  virtual StatusCode matchVertices( const xAOD::VertexContainer & vtxContainer, const xAOD::TruthVertexContainer & truthVtxContainer ) const override;

 private:

  const xAOD::TrackParticleContainer* findTrackParticleContainer( const xAOD::VertexContainer& vxContainer ) const;

  //required MC match probability to consider track a good match
  float m_trkMatchProb;
  //relative weight threshold to consider vertex matched
  float m_vxMatchWeight;
  //pt cut to use on tracks
  float m_trkPtCut;
  //comma separated list of pdgids to consider in the matchiing
  std::string m_pdgIDs;

  mutable std::atomic<unsigned int> m_nVtx {};
  mutable std::atomic<unsigned int> m_nVtxWithBadLinks {};
  mutable std::atomic<unsigned int> m_nBadLinks {};
  mutable std::atomic<unsigned int> m_nLinks {};

  //private methods to check if particles are good to use
  //returns barcode of LLP production truth vertex
  int checkProduction( const xAOD::TruthParticle & truthPart ) const;

  //private internal variables (not properties)
  std::vector<int> m_pdgIDList;

};

#endif
