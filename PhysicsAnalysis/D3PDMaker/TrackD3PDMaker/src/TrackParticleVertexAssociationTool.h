// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackParticleVertexAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Associate from a TrackParticle to its reconstructed vertex.
 */
#ifndef TRACKD3PDMAKER_TRACKPARTICLEVERTEXASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEVERTEXASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "StoreGate/StoreGateSvc.h"

namespace Trk {
  class VxCandidate;
}
namespace Rec {
  class TrackParticle;
}

namespace D3PD {

/**
 * @brief Associate from a TrackParticle to its reconstructed vertex.
 */
class TrackParticleVertexAssociationTool
  : public SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>,
                                 Types<Trk::VxCandidate, xAOD::Vertex> >
{
public:
  typedef SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>,
                                 Types<Trk::VxCandidate, xAOD::Vertex> > Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticleVertexAssociationTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;
  virtual StatusCode book() override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::VxCandidate* get (const Rec::TrackParticle& p) override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::Vertex* get (const xAOD::TrackParticle& p) override;


private:

  /** SG key for target collection. */
  std::string m_targetSGKey;

  /** flag to just return whatever is returned by TrackParticle::reconstructedVertex() */
  bool m_fast;

  /** weight of the track at the associated vertex */
  float *m_weight;

}; // class TrackParticleVertexAssociationTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRACKPARTICLEVERTEXASSOCIATIONTOOL_H
