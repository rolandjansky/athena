/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMaterialAllocator
//  tool interface to allocate tracking geometry material onto indet and/or
//  muon spectrometer tracks
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef TRKIPATFITTERUTILS_IMATERIALALLOCATOR_H
#define TRKIPATFITTERUTILS_IMATERIALALLOCATOR_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk
{
    
/** Interface ID for IMaterialAllocator*/  
static const InterfaceID IID_IMaterialAllocator("IMaterialAllocator", 1, 0);
  
/**@class IMaterialAllocator

Base class for MaterialAllocator AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class FitMeasurement;
class FitParameters;
class TrackStateOnSurface;
    
class IMaterialAllocator : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMaterialAllocator(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMaterialAllocator; }
    
    /**IMaterialAllocator interface: add leading material effects to fit measurements and parameters */
    virtual void	addLeadingMaterial (std::vector<FitMeasurement*>& measurements,
					    ParticleHypothesis		  particleHypothesis,
					    FitParameters&		  fitParameters) const = 0;
    
    /**IMaterialAllocator interface: allocate material */
    virtual void	allocateMaterial (std::vector<FitMeasurement*>&	measurements,
					  ParticleHypothesis		particleHypothesis,
					  const FitParameters&		fitParameters,
					  const TrackParameters&	startParameters) const = 0;

    /**IMaterialAllocator interface: clear temporary TSOS*/
    virtual void	clear (void) = 0;

    /**IMaterialAllocator interface: initialize scattering (needs to know X0 integral) */
    virtual void	initializeScattering (std::vector<FitMeasurement*>& measurements) const = 0;

    /**IMaterialAllocator interface:
       material TSOS between spectrometer entrance surface and parameters given in spectrometer */
    virtual std::vector<const TrackStateOnSurface*>*    leadingSpectrometerTSOS(
	const TrackParameters& spectrometerParameters) const = 0;
    
    /**IMaterialAllocator interface: clear temporary TSOS*/
    virtual void	orderMeasurements (std::vector<FitMeasurement*>& measurements,
					   Amg::Vector3D		 startDirection,
					   Amg::Vector3D		 startPosition) const = 0;
    
    /**IMaterialAllocator interface: has material been reallocated? */   
    virtual bool	reallocateMaterial (std::vector<FitMeasurement*>& measurements,
					    const FitParameters&	  fitParameters) const = 0;

};
 
}	// end of namespace

#endif // TRKIPATFITTERUTILS_IMATERIALALLOCATOR_H


