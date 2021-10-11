/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetVertexTruthMatchTool_h
#define InDetVertexTruthMatchTool_h

// Framework include(s):
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTruth/TruthParticleFwd.h"
#include "InDetTruthVertexValidation/IInDetVertexTruthMatchTool.h"

/** Class for vertex truth matching.
 * Match reconstructed vertices to truth level interactions vertices
 * through the chain: track -> particle -> genEvent -> genVertex
 * Categorize reconstructed vertices depending on their composition.
 */


class InDetVertexTruthMatchTool : public virtual IInDetVertexTruthMatchTool,
                                  public asg::AsgTool {

  ASG_TOOL_CLASS( InDetVertexTruthMatchTool, IInDetVertexTruthMatchTool )

 public:

  InDetVertexTruthMatchTool( const std::string & name );

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override;

  //take const collection of vertices, match them, and decorate with matching info
  virtual StatusCode matchVertices( const xAOD::VertexContainer & vxContainer ) const override;

 private:

  const xAOD::TrackParticleContainer* findTrackParticleContainer( const xAOD::VertexContainer& vxContainer ) const;

  //required MC match probability to consider track a good match
  float m_trkMatchProb;
  //relative weight threshold to consider vertex matched
  float m_vxMatchWeight;
  //pt cut to use on tracks
  float m_trkPtCut;

  mutable std::atomic<unsigned int> m_nVtx {};
  mutable std::atomic<unsigned int> m_nVtxWithBadLinks {};
  mutable std::atomic<unsigned int> m_nBadLinks {};
  mutable std::atomic<unsigned int> m_nLinks {};

  //private methods to check if particles are good to use
  bool pass( const xAOD::TruthParticle & truthPart ) const;
  bool pass( const xAOD::TrackParticle & trackPart ) const;

};

#endif
