/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidVertexCombiner
//  tool interface to match tracks from an input muon spectrometer
//  TrackParticleContainer to vertices in the InnerDetector.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDVERTEXCOMBINER_H
#define MUIDINTERFACES_IMUIDVERTEXCOMBINER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

class MuidTrackContainer;
class VxContainer;

namespace Rec
{

class MuonMatch;
    
/** Interface ID for IMuidVertexCombiner*/  
static const InterfaceID IID_IMuidVertexCombiner("IMuidVertexCombiner", 1, 0);
  
/**@class IMuidVertexCombiner

Base class for MuidVertexCombiner AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidVertexCombiner : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidVertexCombiner(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuidVertexCombiner; }

    /**IMuidVertexCombiner interface:
       algorithmic code for matching tracks in a muon spectrometer TrackCollection
       to vertices in the inDet */
    virtual std::vector<Rec::MuonMatch*>* execute(TrackCollection&		extrapolatedTracks,
						  const MuidTrackContainer*	muonMatches,
						  const VxContainer&		vertices) const = 0;
 
};
 
}	// end of namespace

#endif // MUIDINTERFACES_IMUIDVERTEXCOMBINER_H


