/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRACKDIRECTIONTOOL_H
#define ITRACKDIRECTIONTOOL_H
//
#include "GaudiKernel/AlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"

static const InterfaceID IID_ITrackDirectionTool("ITrackDirectionTool", 1, 0);

namespace Trk { class Track; }
class INavigable4Momentum;

/** @class ITrackDirectionTool

    Tool used to find out the direction of a track in a cosmic run. 
    For tracks (Trk::Track or Rec::TrackParticle) with TrackParameters in the Inner Detector and the Muon Spectrometer,
    determine if the track points at or away from the perigee.
    Implemented in TrackInCaloTools/TrackDirectionTool

    @author Bruno.Lenzi@cern.ch
*/
class ITrackDirectionTool : virtual public IAlgTool
{
  public:
    virtual ~ITrackDirectionTool(){}
    static const InterfaceID&	interfaceID() { return IID_ITrackDirectionTool; };
    
    virtual bool getPropagationDirection(const Trk::Track*, Trk::PropDirection&) =0;
    virtual bool getPropagationDirection(const INavigable4Momentum*, Trk::PropDirection&) =0;
};

#endif
