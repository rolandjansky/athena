// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleInfoAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticleInfoAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its TrackInfo.
 */
#ifndef TRACKD3PDMAKER_TRACKPARTICLEINFOASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEINFOASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace Trk {
  class TrackInfo;
}

namespace Rec {
  class TrackParticle;
}

namespace D3PD {

/**
 * @brief Associate from a TrackParticle to its TrackInfo.
 */
class TrackParticleInfoAssociationTool
  : public SingleAssociationTool<Rec::TrackParticle, Trk::TrackInfo>
{
public:
  typedef SingleAssociationTool<Rec::TrackParticle, Trk::TrackInfo> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticleInfoAssociationTool (const std::string& type,
				       const std::string& name,
				       const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackInfo* get (const Rec::TrackParticle& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRACKPARTICLEINFOASSOCIATIONTOOL_H
