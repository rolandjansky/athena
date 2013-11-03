/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackInCaloTimingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ICALOTIMEOFFLIGHTTOOL_H
#define ICALOTIMEOFFLIGHTTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "TrkToolInterfaces/ITrackTimingTool.h"

namespace Rec
{
  static const InterfaceID IID_ITrackInCaloTimingTool("ITrackInCaloTimingTool", 1, 0);

  /** @class ITrackInCaloTimingTool
    @brief Interface for tools that calculate time-of-flight for tracks and track particles.

    @author Christian Ohm
  */

  class ITrackInCaloTimingTool : virtual public IAlgTool,
                                         Trk::ITrackTimingTool {
  public:
    static const InterfaceID& interfaceID( );

    /** Returns the calculated speed of the particle based on calorimeter timing information.
      @param  Reference to TrackParticle
      @return vector of time measurement pointers
    */
    virtual float calorimeterBeta(const Trk::TrackParticleBase& trk) const = 0;

    /** Returns the calculated speed of the particle based on calorimeter timing information.
      @param  Reference to Track
      @return Measured speed
    */
    virtual float calorimeterBeta(const Trk::Track& trk) const = 0;
      
    // add: methods where cell energy threshold, calorimeters, make samplings configurable
    
  };

  inline const InterfaceID& ITrackInCaloTimingTool::interfaceID(){ 
    return IID_ITrackInCaloTimingTool; 
  }
}

#endif 
