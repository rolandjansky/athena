/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUIDINTERFACES_IMUONTRACKQUERY_H
#define MUIDINTERFACES_IMUONTRACKQUERY_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" //typedef, cannot fwd declare
#include <memory> //for unique_ptr

class CaloEnergy;
namespace Trk
{
    class Track;
}
 
namespace Rec
{
  
class FieldIntegral;
class ScatteringAngleSignificance;
/** Interface ID for IMuonTrackQuery*/  
static const InterfaceID IID_IMuonTrackQuery("IMuonTrackQuery", 1, 0);
  
/**@class IMuonTrackQuery

Base class for MuonTrackQuery AlgTool
     
@author Alan.Poppleton@cern.ch
*/
class IMuonTrackQuery : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuonTrackQuery(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuonTrackQuery; }

    /**IMuonTrackQuery interface:
       caloEnergy from appropriate TSOS */
    virtual const CaloEnergy*	caloEnergy (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       track energy deposit in calorimeters (as fitted or otherwise applied) */
    virtual double		caloEnergyDeposit (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       field integral along track from momentum kick between measurements */
    virtual FieldIntegral	fieldIntegral (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       does track have at least 3 TSOS's representing calorimeter ? */
    virtual bool		isCaloAssociated (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       does track have measurements from indet and spectrometer ? */
    virtual bool		isCombined (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       does track have measurements in spectrometer and parameters but not measurements in indet ? */
    virtual bool		isExtrapolated (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       does track have fitted curvature ? */
    virtual bool		isLineFit (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       is track (roughly) projective towards IP? */
    virtual bool		isProjective (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       is there a long/short chamber overlap? */
    virtual bool		isSectorOverlap (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       does track have TrackParameters at every TSOS ? */
    virtual bool		isSlimmed (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
     significance of momentum balance for combined tracks (biassed residual) */
    virtual double		momentumBalanceSignificance (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
     number of PseudoMeasurements on track (counts one for any vertex measurement) */
    virtual unsigned		numberPseudoMeasurements (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       perigee expressed outgoing from IP */
    virtual const Trk::Perigee*	outgoingPerigee (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
     significance of inner scattering angle pattern for unslimmed tracks (wider than gaussian) */
    virtual ScatteringAngleSignificance	scatteringAngleSignificance (const Trk::Track& track) const = 0;

    /**IMuonTrackQuery interface:
       trackParameters at innermost measurement TSOS in MS */
    //virtual const Trk::TrackParameters*	spectrometerParameters (const Trk::Track& track) const = 0;
    virtual std::unique_ptr<Trk::TrackParameters> spectrometerParameters (const Trk::Track& track) const = 0;
    
    /**IMuonTrackQuery interface:
       assess the number of additional phi measurements needed for MS (or SA) track fit */
    virtual unsigned			spectrometerPhiQuality (const Trk::Track& track) const = 0;
         
    /**IMuonTrackQuery interface:
       trackParameters at innermost trigger chamber TSOS in MS */
    virtual const Trk::TrackParameters*	triggerStationParameters (const Trk::Track& track) const = 0;
    
};
 
}	// end of namespace

#endif // MUIDINTERFACES_IMUONTRACKQUERY_H


