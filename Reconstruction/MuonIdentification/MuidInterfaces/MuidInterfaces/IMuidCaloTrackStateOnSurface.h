/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidCaloTrackStateOnSurface
//  tool interface to allocate scattering centres as TrackStateOnSurface
//  objects representing the Coulomb scattering and energy deposit between 
//  the InDet and MuonSpectrometer entrance.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDCALOTRACKSTATEONSURFACE_H
#define MUIDINTERFACES_IMUIDCALOTRACKSTATEONSURFACE_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk
{
    class TrackStateOnSurface;
}

namespace Rec
{
    
/** Interface ID for IMuidCaloTrackStateOnSurface*/  
static const InterfaceID IID_IMuidCaloTrackStateOnSurface("IMuidCaloTrackStateOnSurface", 1, 0);
  
/**@class IMuidCaloTrackStateOnSurface

Base class for MuidCaloTrackStateOnSurface AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidCaloTrackStateOnSurface : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidCaloTrackStateOnSurface(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuidCaloTrackStateOnSurface; }

    /**IMuidCaloTrackStateOnSurface interface:
       to get the 3 scattering and energy deposit TSOS'es representing the calorimeter.
       The input TrackParameters may be anywhere along the track. */
    virtual std::vector<const Trk::TrackStateOnSurface*>* caloTSOS (
	const Trk::TrackParameters& parameters) const = 0;
    
    /**IMuidCaloTrackStateOnSurface interface:
       to get individually the scattering TSOS'es representing the calorimeter.
       The input TrackParameters may be anywhere along the track. */
    virtual const Trk::TrackStateOnSurface*	innerTSOS (
	const Trk::TrackParameters& parameters) const = 0;
    virtual const Trk::TrackStateOnSurface*	outerTSOS (
	const Trk::TrackParameters& parameters) const = 0;
    
    /**IMuidCaloTrackStateOnSurface interface:
       to get the energy deposit TSOS representing the calorimeter.
       The input TrackParameters may be anywhere along the track, but when the inner
       or outer parameters are provided they must be expressed at the appropriate surface.
       The return TSOS surface is at the material midpoint. */
    virtual const Trk::TrackStateOnSurface*	middleTSOS (
	const Trk::TrackParameters& middleParameters,
	const Trk::TrackParameters* innerParameters = 0,
	const Trk::TrackParameters* outerParameters = 0) const = 0;
};
 
}	// end of namespace

#endif // MUIDINTERFACES_IMUIDCALOTRACKSTATEONSURFACE_H


