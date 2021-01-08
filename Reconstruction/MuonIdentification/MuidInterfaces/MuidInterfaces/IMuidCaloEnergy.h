/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidCaloEnergy
//  tool interface for estimating the energy loss of a muon with its error.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef MUIDINTERFACES_IMUIDCALOENERGY_H
#define MUIDINTERFACES_IMUIDCALOENERGY_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

class CaloEnergy;

namespace Trk
{
    class TrackStateOnSurface;
}

namespace Rec
{
   
/** Interface ID for IMuidCaloEnergy*/  
static const InterfaceID IID_IMuidCaloEnergy("IMuidCaloEnergy", 1, 0);
  
/**@class IMuidCaloEnergy

Base class for MuidCaloEnergy AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidCaloEnergy : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidCaloEnergy(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&			interfaceID() { return IID_IMuidCaloEnergy; }

    /**IMuidCaloEnergy interface:
       to get the total energyLoss in the calorimeters */
    virtual CaloEnergy*				energyLoss (double trackMomentum,
							    double eta,
							    double phi) const = 0;

    /**IMuidCaloEnergy interface:
       TrackStateOnSurface for parameters and energyLoss at the calorimeter mid-surface */
    virtual const Trk::TrackStateOnSurface*	trackStateOnSurface (
	const Trk::TrackParameters& middleParameters,
	const Trk::TrackParameters* innerParameters = 0,
	const Trk::TrackParameters* outerParameters = 0) const = 0;

};

}	// end of namespace

#endif // MUIDINTERFACES_IMUIDCALOENERGY_H


