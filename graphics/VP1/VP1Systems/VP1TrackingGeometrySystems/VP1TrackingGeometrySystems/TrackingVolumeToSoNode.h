/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINGVOLUMETOSONODE_H
#define TRACKINGVOLUMETOSONODE_H

#include "GeoModelKernel/GeoShape.h"
#include "TrkVolumes/VolumeBounds.h"

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class TrackingVolumeToSoNode                       //
//                                                                     //
//  Author: Andreas Salzburger <Andreas.Salzburger@cern.ch> (primary)  //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: June 2007                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

class SoNode;

namespace Trk {
  class TrackingVolume;
}

class TrackingVolumeToSoNode{

public:
  // Public type definitions:

  // Default constructor
  TrackingVolumeToSoNode();

  // Destructor
  virtual ~TrackingVolumeToSoNode();

  // just the TrackingVolume
  SoNode*  translateTrackingVolume(const Trk::TrackingVolume& tvol) const;

private:
  const GeoShape* getShapeFromBounds(const Trk::VolumeBounds* volBounds ) const;

};

#endif


