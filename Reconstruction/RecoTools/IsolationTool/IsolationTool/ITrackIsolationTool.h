/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRKISOLATIONTOOLS_ITRACKISOALTIONTOOL_H
#define TRKISOLATIONTOOLS_ITRACKISOALTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODPrimitives/IsolationType.h"
#include <set>

namespace xAOD {

  static const InterfaceID IID_ITrackIsolationTool("xAOD::ITrackIsolationTool", 1, 0);

  /** @class ITrackIsolationTool
      @brief interface for tools calculating track isolation
 
      @author Niels van Eldik
   */

  struct TrackIsolation {
    std::vector<float> ptcones;
    std::vector<int>   nucones;
    std::vector<const TrackParticle*> tps;
  };

  class ITrackIsolationTool : virtual public IAlgTool {
  public:
    enum SubtractionStrategy {
      UsePointer = 0,
      UseCone,
      NoSubtraction
    };

    static const InterfaceID& interfaceID( ) ;

    /**ITrackIsolationTool interface: 
       Calculates the track isolation. 
       The tool expects the cones to be order in decreasing order (ptcone40 -> ptcone10)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] result              output object to be filled
       @param[in] tp                  input iparticle
       @param[in] cones               vector of input cones to be used
       @param[in] strategy            overlap removal strategy to be used
       @param[in] vertex              if provided vertex to be used for track selection
       @param[in] exclusionSet        if provided track particles in the set are ignored
       @param[in] indetTrackParticles if provided the isolation will be calculated using the ID track particle collection passed
       @return true if the calculation was successfull
     */    
    virtual bool trackIsolation( TrackIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, 
                                 SubtractionStrategy strategy = UsePointer, const Vertex* vertex = 0, 
                                 const std::set<const xAOD::TrackParticle*>* exclusionSet = 0,
                                 const TrackParticleContainer* indetTrackParticles = 0) = 0; 
  };

  inline const InterfaceID& ITrackIsolationTool::interfaceID() { 
    return IID_ITrackIsolationTool; 
  }

} // end of namespace

#endif 
