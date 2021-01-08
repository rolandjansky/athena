/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDCALOMATERIALPARAM_H
#define MUIDINTERFACES_IMUIDCALOMATERIALPARAM_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk
{
    class ITrackingVolumeDisplayer;
    class Surface;
    class TrackStateOnSurface;
}


namespace Rec
{
 
class CaloLayer;
   
/** Interface ID for IMuidCaloMaterialParam*/  
static const InterfaceID IID_IMuidCaloMaterialParam("IMuidCaloMaterialParam", 1, 0);
  
/**@class IMuidCaloMaterialParam

Base class for MuidCaloMaterialParam AlgTool

 The parametrization represents the Coulomb scattering between the InDet
 and MuonSpectrometer entrance.

 Parametrized positions and thicknesses (CaloLayers) are produced
 as a function of eta assuming any track will 'see' 2 scattering centres,
 taken from an inner and an outer layer.
     
@author Alan.Poppleton@cern.ch
*/
class IMuidCaloMaterialParam : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidCaloMaterialParam(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&			interfaceID() { return IID_IMuidCaloMaterialParam; }

    /**IMuidCaloMaterialParam interface:
       return inner/middle/outer surface corresponding to eta value */
    virtual const Trk::Surface*			innerSurface  (double eta) const = 0;
    virtual const Trk::Surface*			middleSurface (double eta) const = 0;
    virtual const Trk::Surface*			outerSurface  (double eta) const = 0;
  
    /**IMuidCaloMaterialParam interface:
       calorimeter layer radiation thickness corresponding to eta value */
    virtual double				radiationThickness (double eta) const = 0;

    /**IMuidCaloMaterialParam interface:
       TrackStateOnSurface for parameters at a scattering surface */
    virtual const Trk::TrackStateOnSurface*	trackStateOnSurface (
	const Trk::TrackParameters*) const = 0;

};
 
}	// end of namespace

#endif // MUIDINTERFACES_IMUIDCALOMATERIALPARAM_H


