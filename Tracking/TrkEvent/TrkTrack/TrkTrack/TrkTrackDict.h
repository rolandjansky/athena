/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKDICT_H
#define TRKTRACKDICT_H

//
// D. Costanzo. These are forward declarations in Track.h
// #include is needed for gcc_xml to parse the header files
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/LinkToTrack.h"
#include "Identifier/Identifier.h"
#include "AthLinks/ElementLink.h"

// We need to instantiate some templates here to be able to create the
// dictionary for them

namespace Trk {
    class TrkTrackDict_Dummy
    {

	      DataVector<const TrackParameters>           m_parameterVector;
	      DataVector<const TrackStateOnSurface>       m_trackStateVector;
	      DataVector<Identifier>                      m_identifierVector;
	      ElementLink<DataVector<Trk::Track> >        m_Elink;   
        std::bitset< 11 >                           m_dummy1;
        TrackCollection m_tc;
        ElementLink<TrackCollection> m_e1;
        //!< really for xAOD, but exposes dependency on TrkTrack to put it there...
        std::vector<ElementLink<TrackCollection> > m_e2;
    };
}
#endif
