/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMATRKREFITTERTOOL_H
#define EGAMMAINTERFACES_IEGAMMATRKREFITTERTOOL_H
/** @brief
  @class IxAODEgammaTrackRefitterTool
          Iterface for track refitter
  @author A. Morley
*/

/********************************************************************

NAME:     IxAODEgammaTrkRefitterTool.h
PACKAGE:  offline/Reconstruction/xAODEgamma/xAODEgammaInterfaces

CREATED:  Jul 2008

********************************************************************/

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_egammaTrkRefitterTool("IegammaTrkRefitterTool", 1, 0);

#include "TrkParameters/TrackParameters.h"
// Forward declarations
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
namespace Trk{
    class Track;
}

class IegammaTrkRefitterTool : virtual public IAlgTool 
{
    public:
    /** Alg tool and IAlgTool interface method */
    static const InterfaceID& interfaceID() 
      { return IID_egammaTrkRefitterTool; };
    
    /** Refit the track associated with an xAODEgamma object*/
    virtual StatusCode  refitElectronTrack(const xAOD::Electron*) = 0;
    
    /** Refit the track associated with a track particle  object*/
    virtual StatusCode  refitTrackParticle(const xAOD::TrackParticle*, const xAOD::Electron* eg = 0  ) = 0;
    
    /** Refit the track*/
		virtual StatusCode  refitTrack(const Trk::Track*, const xAOD::Electron* eg = 0  ) = 0;

    /** Returns the refitted track */
    virtual Trk::Track* refittedTrack() = 0;

    /** Returns the refitted track */
    virtual const Trk::Perigee* refittedTrackPerigee() = 0;

    /** Returns the original track perigee*/
    virtual const Trk::Track* originalTrack() = 0;
    
    /** Returns the original track perigee*/
    virtual const Trk::Perigee* originalTrackPerigee() = 0;    

    /** Returns the refitted track end measurement parameters*/
    virtual const Trk::TrackParameters* refittedEndParameters() = 0;    

    /** Returns the orginal track last measurement parameters*/
    virtual const Trk::TrackParameters* originalEndParameters() = 0;    
  
    /** Resets the track fitter*/
    virtual void resetRefitter() = 0;

};
#endif
