/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_IPVTRACKCOMPATIBILITYESTIMATOR_H
#define TRK_IPVTRACKCOMPATIBILITYESTIMATOR_H


#include "GaudiKernel/IAlgTool.h" 
#include "TrkParameters/TrackParameters.h"

/**
 * IPVTrackCompatibilityEstimator.h
 * 
 * Abstract interface for tools estimating whether a given track 
 * originated from the primary vertex.
 *
 * ruwiedel@physik.uni-bonn.de
 * Kirill.Prokofiev@cern.ch 
 * 
 * June 2009
 */


class VxContainer;

namespace Trk
{

  class Track;
  class TrackParticleBase;

 static const InterfaceID IID_IPVTrackCompatibilityEstimator("IPVTrackCompatibilityEstimator", 1, 0);
 
 class IPVTrackCompatibilityEstimator : virtual public IAlgTool
 {
    
  public:

    virtual ~IPVTrackCompatibilityEstimator(){};
    
    static const InterfaceID& interfaceID(){return IID_IPVTrackCompatibilityEstimator;}
    
    virtual bool isCompatible(const Trk::Track* track,
                              const VxContainer* pvContainer, unsigned int pvIndex = 0) const = 0;
    
    virtual bool isCompatible(const Trk::TrackParticleBase* track,
                              const VxContainer* pvContainer, unsigned int pvIndex = 0) const = 0;

    virtual bool isCompatible(const TrackParameters* track,
                              const VxContainer* pvContainer, unsigned int pvIndex = 0) const = 0;
    
 }; 
  
}//end of namespace definitions

#endif 
