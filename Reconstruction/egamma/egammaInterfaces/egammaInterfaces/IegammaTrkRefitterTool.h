/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/EventContext.h"

#include "TrkParameters/TrackParameters.h"
// Forward declarations
#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "TrkTrack/Track.h"
#include <memory>

static const InterfaceID IID_egammaTrkRefitterTool("IegammaTrkRefitterTool", 1, 0);

class IegammaTrkRefitterTool : virtual public IAlgTool 
{
    public:
    /** Alg tool and IAlgTool interface method */
    static const InterfaceID& interfaceID() 
      { return IID_egammaTrkRefitterTool; };
  
    /** Struct Holding the result to return and intermediate objects
     *  Things are owned by the EDM or the unique_ptr*/
    struct Cache {
      /** @brief Pointer to the refitted track*/  
      std::unique_ptr<Trk::Track>   refittedTrack; 
      /** @brief Pointer to the refitted MeasuredPerigee*/    
      const Trk::Perigee* refittedTrackPerigee;
      /** @brief Pointer to the original track*/  
      const Trk::Track*    originalTrack; 
      /** @brief Pointer to the original Perigee*/    
      const Trk::Perigee*  originalTrackPerigee;
     /** @brief pointer to the Electron input*/
      const xAOD::Electron* electron; 
      Cache():refittedTrack(nullptr),
      refittedTrackPerigee(nullptr),
      originalTrack(nullptr),
      originalTrackPerigee(nullptr),
      electron(nullptr){
      }
    };

    /** Refit the track*/
		virtual StatusCode  refitTrack(const EventContext& ctx,
                                   const Trk::Track*, 
                                   Cache& result) const = 0;

};
#endif
