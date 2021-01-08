/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidMatchMaker
//  tool interface to match tracks from an extrapolated muon-spectrometer
//  collection to any corresponding tracks from an inner detector collection.
//  Perform a full track-fit to the resultant combined tracks.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDMATCHMAKER_H
#define MUIDINTERFACES_IMUIDMATCHMAKER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"

class PerigeeParameters;
class MuidTrackContainer;


namespace Rec
{
  
/** Interface ID for IMuidMatchMaker*/  
static const InterfaceID IID_IMuidMatchMaker("IMuidMatchMaker", 1, 0);
  
/**@class IMuidMatchMaker

Base class for MuidMatchMaker AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidMatchMaker : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidMatchMaker(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&		interfaceID() { return IID_IMuidMatchMaker; }

    /**IMuidMatchMaker interface:
       back convert perigee (legacy function needed during migration) */
    virtual PerigeeParameters*		convertPerigee (const Trk::Perigee* trkPerigee) const = 0;
    
    /**IMuidMatchMaker interface:
       algorithmic code for matching muon spectrometer to inner detector */
    virtual MuidTrackContainer*		execute(
	const TrackCollection*		extrapolatedTracks,
	const TrackCollection*		indetTracks,
	const TrackCollection*		spectrometerTracks) = 0;
};
 
}	// end of namespace

#endif // MUIDINTERFACES_IMUIDMATCHMAKER_H


