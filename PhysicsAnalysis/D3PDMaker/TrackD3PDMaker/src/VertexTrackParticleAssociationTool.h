// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_VERTEXTRACKPARTICLEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_VERTEXTRACKPARTICLEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#include <vector>

namespace Rec{
class TrackParticle;
}

namespace Trk{
class VxCandidate;
class VxTrackAtVertex;
}

namespace D3PD {


class VertexTrackParticleAssociationTool
  : public MultiAssociationTool<Types<Trk::VxCandidate, xAOD::Vertex>,
                                Types<Rec::TrackParticle, xAOD::TrackParticle> >
{
public:
  typedef MultiAssociationTool<Types<Trk::VxCandidate, xAOD::Vertex>,
                               Types<Rec::TrackParticle, xAOD::TrackParticle> > Base;
  using Base::next;

  VertexTrackParticleAssociationTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book() override;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Trk::VxCandidate& p) override;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const xAOD::Vertex& p) override;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const Rec::TrackParticle* next (const Trk::VxCandidate* dummy) override;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const xAOD::TrackParticle* next (const xAOD::Vertex* dummy) override;


private:
  /// Variable: Track weight.
  float* m_weight;

  const Trk::VxCandidate* m_aod_vx;
  const xAOD::Vertex* m_xaod_vx;
  size_t m_itrack;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_VERTEXTRACKPARTICLEASSOCIATIONTOOL_H
