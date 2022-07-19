// -*- c++ -*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_ITRACKFILTERTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_ITRACKFILTERTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODTracking/TrackParticleFwd.h"
#include <string>

namespace InDet {

  /// @class InclusiveTrackFilterTool
  /// This tool randomly discards tracks based on the TCP efficiency
  /// systematics histograms
  /// 
  /// Based on the JetInclusiveTrackFilterTool
  /// 
  /// @author Jackson Burzynski (jackson.carl.burzynski@cern.ch)

  class IInclusiveTrackFilterTool
    : public virtual asg::IAsgTool
    , public virtual CP::ISystematicsTool
  {
      
  public:
    ASG_TOOL_INTERFACE( InDet::IInclusiveTrackFilterTool )
	
    virtual bool accept( const xAOD::TrackParticle* ) const = 0;
      
  }; // class IInclusiveTrackFilterTool

} // namespace InDet

#endif
