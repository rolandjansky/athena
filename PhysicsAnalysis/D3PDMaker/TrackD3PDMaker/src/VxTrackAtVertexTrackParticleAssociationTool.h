// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VxTrackAtVertexTrackParticleAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VxTrackAtVertexTrackParticleAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a VxTrackAtVertex to a TrackParticle.
 */
#ifndef TRACKD3PDMAKER_VXTRACKATVERTEXTRACKPARTICLEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_VXTRACKATVERTEXTRACKPARTICLEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace Rec {
  class TrackParticle;
}

namespace Trk {
  class VxTrackAtVertex;
}

namespace D3PD {

/**
 * @brief Associate from a VxTrackAtVertex to a TrackParticle.
 */
class VxTrackAtVertexTrackParticleAssociationTool
  : public SingleAssociationTool<Trk::VxTrackAtVertex, Rec::TrackParticle>
{
public:
  typedef SingleAssociationTool<Trk::VxTrackAtVertex, Rec::TrackParticle> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VxTrackAtVertexTrackParticleAssociationTool (const std::string& type,
					       const std::string& name,
					       const IInterface* parent);

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Rec::TrackParticle* get (const Trk::VxTrackAtVertex& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_VXTRACKATVERTEXTRACKPARTICLEASSOCIATIONTOOL_H
