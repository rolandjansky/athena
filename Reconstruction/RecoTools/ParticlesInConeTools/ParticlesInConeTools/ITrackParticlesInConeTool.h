/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITracksInConeTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PARTICLESINCONETOOLS_ITRACKPARTICLESINCONETOOL_H
#define PARTICLESINCONETOOLS_ITRACKPARTICLESINCONETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include <vector>

namespace xAOD {

  static const InterfaceID IID_ITrackParticlesInConeTool("xAOD::ITrackParticlesInConeTool", 1, 0);

  /** @class ITrackParticlesInConeTool
      @brief interface for collecting tracks inside a code
 
      @author Niels van Eldik
   */
  class ITrackParticlesInConeTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**ITrackParticlesInConeTool interface: 
       @param[in] eta       eta for matching
       @param[in] phi       phi for matching
       @param[in] dr        cone size
       @param[in] output    output vector to be filled
       @return true if the calculation was successfull
     */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output ) = 0;
  };

  inline const InterfaceID& ITrackParticlesInConeTool::interfaceID() { 
    return IID_ITrackParticlesInConeTool; 
  }

} // end of namespace

#endif 
