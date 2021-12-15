/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloTrackingVolumeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ICALOTRACKINGVOLUMEBUILDER_H
#define TRKDETDESCRINTERFACES_ICALOTRACKINGVOLUMEBUILDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

class CaloDetDescrManager;
namespace Trk {

class TrackingVolume;
class BinUtility1D;

/** Interface ID for ICaloTrackingVolumeBuilders*/
static const InterfaceID
  IID_ICaloTrackingVolumeBuilder("ICaloTrackingVolumeBuilder", 1, 0);

/** @class ICaloTrackingVolumeBuilder
  Interface class ICaloTrackingVolumeBuilders
  It inherits from IAlgTool. The actual implementation of the AlgTool depends on
  the SubDetector, more detailed information should be found there.
  */
class ICaloTrackingVolumeBuilder : virtual public IAlgTool
{

public:
  /**Virtual destructor*/
  virtual ~ICaloTrackingVolumeBuilder() {}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID()
  {
    return IID_ICaloTrackingVolumeBuilder;
  }

  /** TrackingVolumeBuilder interface method - returns vector of Volumes */
  virtual const std::vector<TrackingVolume*>* trackingVolumes(
    const CaloDetDescrManager& caloDDM) const = 0;
};

} // end of namespace

#endif
