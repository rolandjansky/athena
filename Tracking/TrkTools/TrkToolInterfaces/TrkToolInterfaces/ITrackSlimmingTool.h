/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackSlimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKSLIMMINGTOOL_H
#define ITRKTRACKSLIMMINGTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace Trk 
    
{
    class Track;

    static const InterfaceID IID_ITrackSlimmingTool("Trk::ITrackSlimmingTool", 1, 0);

/** @brief Interface for constructing 'slimmed' Tracks from complete tracks.

    @author edward.moyse@cern.ch
*/
    class ITrackSlimmingTool : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID( ) ;

      /**This method 'skims' interesting information from the passed track, and creates a new one with cloned copies of this information
        @param track A reference to the track to be skimmed. It will not be modified in any way.
        @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is configured*/
        virtual Trk::Track* slim(const Trk::Track& track)=0;

    };

inline const InterfaceID& Trk::ITrackSlimmingTool::interfaceID()
{ 
    return IID_ITrackSlimmingTool; 
}

} // end of namespace

#endif 
