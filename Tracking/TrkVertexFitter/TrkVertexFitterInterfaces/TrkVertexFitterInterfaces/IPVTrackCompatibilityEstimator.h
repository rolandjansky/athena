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
 *
 * ---------------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *
 *   EDM Migration to xAOD - Replace Trk::VxContainer with xAOD::VertexContainer
 *
 */

namespace Trk
{

 static const InterfaceID IID_IPVTrackCompatibilityEstimator("IPVTrackCompatibilityEstimator", 1, 0);
 
 class IPVTrackCompatibilityEstimator : virtual public IAlgTool
 {
    
  public:

    virtual ~IPVTrackCompatibilityEstimator(){};
    
    static const InterfaceID& interfaceID(){return IID_IPVTrackCompatibilityEstimator;}
    
    virtual bool isCompatible(const xAOD::TrackParticle* track,
                              const xAOD::VertexContainer* pvContainer, unsigned int pvIndex = 0) const = 0;

    virtual bool isCompatible(const TrackParameters* track,
                              const xAOD::VertexContainer* pvContainer, unsigned int pvIndex = 0) const = 0;
    
 }; 
  
}//end of namespace definitions

#endif 
