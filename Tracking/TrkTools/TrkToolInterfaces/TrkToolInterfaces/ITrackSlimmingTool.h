/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackSlimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKSLIMMINGTOOL_H
#define ITRKTRACKSLIMMINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "CxxUtils/checker_macros.h"

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
        /**This method 'skims' interesting information from the passed track, and creates a 
         * new one with cloned copies of this information
         * When m_setPersistificationHints = False
         @param track A const reference to the track to be skimmed. It will not be modified in any way.
         @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is configured
         * When m_setPersistificationHints = True
         @param track A reference to the track to be skimmed.It gets modified by setting persistification hints
         @return nullptr
         The later behaviour can be not thread-safe , look method slimCopy below 
         */
        virtual Trk::Track* slim ATLAS_NOT_THREAD_SAFE (const Trk::Track& track) const = 0;

        /**This method always creates a std::unique_ptr<Trk::Track*> with information removed
         * based on the tool configuration (m_setPersistificationHints is not used)
          @param track A const reference to the track to be skimmed. It will not be modified in any way.
          @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is configured
          */
        virtual std::unique_ptr<Trk::Track> slimCopy(const Trk::Track& track) const=0;

       /**
        * Slim/skim a non const Track. (m_setPersistificationHints is not used)
        * @param track A reference to the track to be skimmed. It will be modified.
        */
        virtual void slimTrack(Trk::Track& track) const = 0;
    };

inline const InterfaceID& Trk::ITrackSlimmingTool::interfaceID()
{ 
    return IID_ITrackSlimmingTool; 
}

} // end of namespace

#endif 
