/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidBackTracker
//  tool interface to back-track all tracks in an input muon spectrometer
//  TrackCollection to the IP.
//  The return collection is required to have a one-to-one correspondence
//  to the input.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDBACKTRACKER_H
#define MUIDINTERFACES_IMUIDBACKTRACKER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

namespace Rec
{
    
/** Interface ID for IMuidBackTracker*/  
static const InterfaceID IID_IMuidBackTracker("IMuidBackTracker", 1, 0);
  
/**@class IMuidBackTracker

Base class for MuidBackTracker AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidBackTracker : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidBackTracker(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuidBackTracker; }

    /**IMuidBackTracker interface:
       algorithmic code for back-tracking tracks in a muon spectrometer TrackCollection
       to the beam intersection point */
    virtual TrackCollection*	execute(const TrackCollection& spectrometerTracks) const = 0;
 
};
 
}	// end of namespace

#endif // MUIDINTERFACES_IMUIDBACKTRACKER_H


