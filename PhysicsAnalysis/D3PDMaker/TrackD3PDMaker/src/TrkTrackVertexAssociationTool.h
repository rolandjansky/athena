// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrkTrackVertexAssociationTool.h 502118 2012-05-23 19:10:19Z ssnyder $
/**
 * @file TrackD3PDMaker/src/TrkTrackVertexAssociationTool.h
 * @author Remi Zaidan <zaidan@cern.ch>
 * @date Mar, 2011
 * @brief Associate from a Trk::Track to its reconstructed vertex.
 */
#ifndef TRACKD3PDMAKER_TRKTRACKVERTEXASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRKTRACKVERTEXASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "StoreGate/StoreGateSvc.h"

namespace Trk {
  class VxCandidate;
  class Track;
}

namespace D3PD {

/**
 * @brief Associate from a TrkTrack to its reconstructed vertex.
 */
class TrkTrackVertexAssociationTool
  : public SingleAssociationTool<Trk::Track,Trk::VxCandidate>
{
public:
  typedef SingleAssociationTool<Trk::Track,Trk::VxCandidate> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrkTrackVertexAssociationTool (const std::string& type,
				 const std::string& name,
				 const IInterface* parent);

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();
  virtual StatusCode book();

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::VxCandidate* get (const Trk::Track& p);

private:

  /** SG key for target collection. */
  std::string m_targetSGKey;

  /** needed for common python interface with TrackParticleVertexAssociationTool */
  bool m_fast;

  /** weight of the track at the associated vertex */
  float *m_weight;

}; // class TrkTrackVertexAssociationTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKVERTEXASSOCIATIONTOOL_H
