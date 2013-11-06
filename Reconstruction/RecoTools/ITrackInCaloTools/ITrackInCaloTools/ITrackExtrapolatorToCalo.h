/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRACKEXTRAPOLATORTOCALO_H
#define ITRACKEXTRAPOLATORTOCALO_H
//
#include "GaudiKernel/AlgTool.h"
#include "TrkEventPrimitives/PropDirection.h" // cannot forward declare because it is an enum
#include "TrkParameters/TrackParameters.h" // cannot forward declare because it is a typedef
#include <vector>

namespace Trk { class Track; }
class INavigable4Momentum;

static const InterfaceID IID_ITrackExtrapolatorToCalo("ITrackExtrapolatorToCalo", 1, 0);

/** @class ITrackExtrapolatorToCalo

    Tool used to extrapolate a track from the perigee to each layer of the calorimeters
    Implemented in TrackInCaloTools/TrackExtrapolatorToCalo

    @author Bruno.Lenzi@cern.ch
*/
class ITrackExtrapolatorToCalo : virtual public IAlgTool
{
  public:
    virtual ~ITrackExtrapolatorToCalo(){}
    static const InterfaceID&	interfaceID() { return IID_ITrackExtrapolatorToCalo; };
    
    /** Extrapolate the track to the entrance of each layer, save the results in m_extrapolation results and
    return a vector with one <const TrackParameter*> per layer (entry is null if extrapolation failed) **/
    virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const Trk::TrackParameters*) =0;
    virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const Trk::Track*) =0;
    virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const INavigable4Momentum*) =0;
    
    /** Return the extrapolations to the exit of each layer **/
    virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const Trk::TrackParameters*) =0;
    virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const Trk::Track*) =0;
    virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const INavigable4Momentum*) =0;
    
    /** Return a vector with the path length of the track on each layer. For the layers where the extrapolation failed, path length = 0 **/
    virtual std::vector<double> getPathLengths(const Trk::TrackParameters*) =0;
    virtual std::vector<double> getPathLengths(const Trk::Track*) =0;
    virtual std::vector<double> getPathLengths(const INavigable4Momentum*) =0;

    /** Given the TrackParameters, return a vector< pair<double, double> > with the mid-point between the extrapolations
    for the entrance and the exit of each layer. When the extrapolation failed for the exit of the layer, 
    take the position in the entrance. When it fails for both, take the one for the previous layer 
    (or the track direction) **/
    virtual std::vector< std::pair<double, double> > getMidPointForExtrapolations(const Trk::TrackParameters*) =0;
    virtual std::vector< std::pair<double, double> > getMidPointForExtrapolations(const Trk::Track*) =0;
    virtual std::vector< std::pair<double, double> > getMidPointForExtrapolations(const INavigable4Momentum*) =0;
    
    /** Return the parametrized energy loss per layer (difference between the momentum at the entrance and exit).
    returns E, not Et **/
    virtual std::vector<double> getParametrizedEloss(const Trk::TrackParameters* track) =0;
    virtual std::vector<double> getParametrizedEloss(const Trk::Track* track) =0;
    virtual std::vector<double> getParametrizedEloss(const INavigable4Momentum* track) =0;
    
    /** Set the propagation direction (along or oppositeMomentum), has to be determined outside if using TrackParameters **/
    virtual void setPropagationDirection(Trk::PropDirection direction) =0;
    /** Return the current propagation direction (along or oppositeMomentum) **/
    virtual int getPropagationDirection() =0;  
    /** Return TrackParameters at perigee for given Trk::Track. Set propagation direction for cosmics **/
    virtual const Trk::TrackParameters* getTrackParameters(const Trk::Track *track) =0;
    /** Return TrackParameters at perigee for given INavigable4Momentum. Set propagation direction for cosmics **/
    virtual const Trk::TrackParameters* getTrackParameters(const INavigable4Momentum *track) =0;

};

#endif
