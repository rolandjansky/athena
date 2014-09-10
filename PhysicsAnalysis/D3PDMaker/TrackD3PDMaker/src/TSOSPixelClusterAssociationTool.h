// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TSOSPixelClusterAssociationTool.h 349056 2011-03-03 13:36:07Z zaidan $
/**
 * @file TrackD3PDMaker/src/TSOSPixelClusterAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Mar, 2011
 * @brief Associate from a TrackParticle to its default Perigee.
 */
#ifndef TRACKD3PDMAKER_TSOSPIXELCLUSTERASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TSOSPIXELCLUSTERASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace Trk {
  class TrackStateOnSurface;
}
namespace InDet {
  class PixelCluster;
}
class AtlasDetectorID;

namespace D3PD {

/**
 * @brief Associate from a pixel TrackStateOnSurface to a PixelCluster.
 */
class TSOSPixelClusterAssociationTool
  : public SingleAssociationTool<Trk::TrackStateOnSurface, InDet::PixelCluster>
{
public:
  typedef SingleAssociationTool<Trk::TrackStateOnSurface, InDet::PixelCluster> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TSOSPixelClusterAssociationTool (const std::string& type,
				   const std::string& name,
				   const IInterface* parent);

  virtual StatusCode initialize();

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   * Return the target of the association, or 0.
   */
  virtual const InDet::PixelCluster* get (const Trk::TrackStateOnSurface& p);

private:

  const AtlasDetectorID* m_idHelper;

}; // class TSOSPixelClusterAssociationTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TSOSPIXELCLUSTERASSOCIATIONTOOL_H
