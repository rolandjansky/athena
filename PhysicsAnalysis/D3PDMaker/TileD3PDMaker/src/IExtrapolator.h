/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   IExtrapolator.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Extrapolator tools interface.
 *
 * Created in February 2013, based on TrackExtrapolatorToCalo.
 */

#ifndef IExtrapolator_H
#define IExtrapolator_H

/// TRACK INCLUDES
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/PropDirection.h"

/// C++ INCLUDES
#include <vector>
#include <math.h>
#include <string>
#include <cmath>

namespace Trk{
    class Track;
}
class INavigable4Momentum;

static const InterfaceID IID_IExtrapolator("IExtrapolator", 1, 0);

//============================================
class IExtrapolator : virtual public IAlgTool{
//============================================

    public:
        virtual ~IExtrapolator(){}
        static const InterfaceID&	interfaceID() { return IID_IExtrapolator; };

        // EXTRAPOLATE TO ENTRANCE OF EACH LAYER
        virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const Trk::TrackParameters*) = 0;
        virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const Trk::Track*) = 0;
        virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const INavigable4Momentum*) = 0;

        // EXTRAPOLATE TO EXIT OF EACH LAYER
        virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const Trk::TrackParameters*) = 0;
        virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const Trk::Track*) = 0;
        virtual std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const INavigable4Momentum*) = 0;

        // CALCULATE PATH LENGTHS IN EACH LAYER
        virtual std::vector<double> getPathLengths(const Trk::TrackParameters*) = 0;
        virtual std::vector<double> getPathLengths(const Trk::Track*) = 0;
        virtual std::vector<double> getPathLengths(const INavigable4Momentum*) = 0;

    /** Given the TrackParameters, return a vector< pair<double, double> > with the mid-point between the extrapolations
    for the entrance and the exit of each layer. When the extrapolation failed for the exit of the layer, 
    take the position in the entrance. When it fails for both, take the one for the previous layer 
    (or the track direction) **/
    virtual std::vector< std::pair<double, double> > getMidPointForExtrapolations(const Trk::TrackParameters*) = 0;
    virtual std::vector< std::pair<double, double> > getMidPointForExtrapolations(const Trk::Track*) = 0;
    virtual std::vector< std::pair<double, double> > getMidPointForExtrapolations(const INavigable4Momentum*) = 0;

    virtual std::vector< std::vector<double> > getTrackPositions(const Trk::TrackParameters* track) = 0;
    virtual std::vector< std::vector<double> > getTrackPositions(const Trk::Track*) = 0;
    virtual std::vector< std::vector<double> > getTrackPositions(const INavigable4Momentum*) = 0;
    
    /** Return the parametrized energy loss per layer (difference between the momentum at the entrance and exit).
    returns E, not Et **/
    virtual std::vector<double> getParametrizedEloss(const Trk::TrackParameters* track) = 0;
    virtual std::vector<double> getParametrizedEloss(const Trk::Track* track) = 0;
    virtual std::vector<double> getParametrizedEloss(const INavigable4Momentum* track) = 0;
    
    /** Set the propagation direction (along or oppositeMomentum), has to be determined outside if using TrackParameters **/
    virtual void setPropagationDirection(Trk::PropDirection direction) = 0;
    /** Return the current propagation direction (along or oppositeMomentum) **/
    virtual int getPropagationDirection() = 0;  
    /** Return TrackParameters at perigee for given Trk::Track. Set propagation direction for cosmics **/
    virtual const Trk::TrackParameters* getTrackParameters(const Trk::Track *track) = 0;
    /** Return TrackParameters at perigee for given INavigable4Momentum. Set propagation direction for cosmics **/
    virtual const Trk::TrackParameters* getTrackParameters(const INavigable4Momentum *track) = 0;

};

#endif
