/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackingVolumeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ITRACKINGVOLUMEBUILDER_H
#define TRKDETDESCRINTERFACES_ITRACKINGVOLUMEBUILDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Trk {

class TrackingVolume;
class BinUtility1D;

/** Interface ID for ITrackingVolumeBuilders*/
static const InterfaceID IID_ITrackingVolumeBuilder("ITrackingVolumeBuilder",
                                                    1,
                                                    0);

/** @class ITrackingVolumeBuilder

  Interface class ITrackingVolumeBuilders
  It inherits from IAlgTool. The actual implementation of the AlgTool depends on
  the SubDetector, more detailed information should be found there.

   @author Andreas.Salzburger@cern.ch
  */
class ITrackingVolumeBuilder : virtual public IAlgTool
{

public:
  /**Virtual destructor*/
  virtual ~ITrackingVolumeBuilder() {}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITrackingVolumeBuilder; }

  /** TrackingVolumeBuilder interface method - returns vector of Volumes */
  virtual const std::vector<TrackingVolume*>* trackingVolumes() const = 0;
};

} // end of namespace

#endif // TRKDETDESCRINTERFACES_IITRACKINGVOLUMEBUILDER_H
