/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IDetachedTrackingVolumeBuilder.h (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_IDETACHEDTRACKINGVOLUMEBUILDER_H
#define TRKDETDESCRINTERFACES_IDETACHEDTRACKINGVOLUMEBUILDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>

namespace Trk {

  class DetachedTrackingVolume;
 
  /** Interface ID for IDetachedTrackingVolumeBuilders*/  
  static const InterfaceID IID_IDetachedTrackingVolumeBuilder("IDetachedTrackingVolumeBuilder", 1, 0);
  
  /** @class IDetachedTrackingVolumeBuilder
    
    Interface class IDetachedTrackingVolumeBuilder,
    the DetachedTrackingVolumeBuilder inherits from this one.
        
    @author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
    */
  class IDetachedTrackingVolumeBuilder : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~IDetachedTrackingVolumeBuilder(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IDetachedTrackingVolumeBuilder; }

      virtual const std::vector<const DetachedTrackingVolume*>* buildDetachedTrackingVolumes(bool blend=false) const = 0 ;
    
  };

} // end of namespace


#endif // TRKDETDESCRINTERFACES_IDETACHEDTRACKINGVOLUMEBUILDER_H


