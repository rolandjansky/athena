/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IDetachedTrackingVolumeBuilderCond.h (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_IDETACHEDTRACKINGVOLUMEBUILDERCOND_H
#define TRKDETDESCRINTERFACES_IDETACHEDTRACKINGVOLUMEBUILDERCOND_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/EventIDRange.h"
// STL
#include <vector>

namespace Trk {

  class DetachedTrackingVolume;
 
  /** Interface ID for IDetachedTrackingVolumeBuilderConds*/  
  static const InterfaceID IID_IDetachedTrackingVolumeBuilderCond("IDetachedTrackingVolumeBuilderCond", 1, 0);
  
  /** @class IDetachedTrackingVolumeBuilderCond
    
    Interface class IDetachedTrackingVolumeBuilderCond,
    the DetachedTrackingVolumeBuilder inherits from this one.
        
    @author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
    */
  class IDetachedTrackingVolumeBuilderCond : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~IDetachedTrackingVolumeBuilderCond(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IDetachedTrackingVolumeBuilderCond; }

      virtual std::pair<EventIDRange, std::unique_ptr<const std::vector<std::unique_ptr<const DetachedTrackingVolume> > > > buildDetachedTrackingVolumes(const EventContext& ctx, bool blend=false ) const = 0 ;
    
  };

} // end of namespace


#endif // TRKDETDESCRINTERFACES_IDETACHEDTRACKINGVOLUMEBUILDERCOND_H


