/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRKISOLATIONTOOLS_ITRACKISOLATIONTOOL_H
#define TRKISOLATIONTOOLS_ITRACKISOLATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODPrimitives/IsolationType.h"
#include "IsolationCommon.h"
#include <set>

namespace xAOD {

  /** @class ITrackIsolationTool
      @brief interface for tools calculating track isolation
 
      @author Niels van Eldik, Sandrine Laplace
  */

  class ITrackIsolationTool : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE( xAOD::ITrackIsolationTool )
  public:

    /**ITrackIsolationTool interface: 
       Calculates the track isolation. 
       The tool expects the cones to be order in decreasing order (ptcone40 -> ptcone10)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] result              output object to be filled
       @param[in] tp                  input iparticle
       @param[in] cones               vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @param[in] vertex              if provided vertex to be used for track selection
       @param[in] exclusionSet        if provided track particles in the set are ignored
       @param[in] indetTrackParticles if provided the isolation will be calculated using the ID track particle collection passed
       @return true if the calculation was successfull
    */    
    virtual bool trackIsolation( TrackIsolation& result, const IParticle& tp, 
				 const std::vector<Iso::IsolationType>& cones, 
                                 TrackCorrection corrections,
				 const Vertex* vertex = 0, 
                                 const std::set<const xAOD::TrackParticle*>* exclusionSet = 0,
                                 const TrackParticleContainer* indetTrackParticles = 0) const = 0; 

    /**ITrackIsolationTool interface: 
       Calculates the track isolation and decorates the particle 
       The tool expects the cones to be order in decreasing order (ptcone40 -> ptcone10)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] tp                  input iparticle
       @param[in] cones               vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @param[in] vertex              if provided vertex to be used for track selection
       @param[in] exclusionSet        if provided track particles in the set are ignored
       @param[in] indetTrackParticles if provided the isolation will be calculated using the ID track particle collection passed
       @return true if the calculation and decoration was successfull
    */    
    virtual bool decorateParticle( IParticle& tp, 
                                   const std::vector<Iso::IsolationType>& cones, 
                                   TrackCorrection corrections, 
                                   const Vertex* vertex = 0, 
                                   const std::set<const TrackParticle*>* exclusionSet = 0, 
                                   const TrackParticleContainer* indetTrackParticles = 0 ) const = 0; 

  };

} // end of namespace

#endif 
