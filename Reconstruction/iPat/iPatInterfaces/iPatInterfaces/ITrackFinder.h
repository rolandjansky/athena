/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackFinder.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRACKFINDER_H
# define IPATINTERFACES_ITRACKFINDER_H

#include <list>
#include <vector>
#include "GaudiKernel/IAlgTool.h"

// using iPatTrack family for now ...
class Track;
class VertexRegion;
namespace Trk
{
    class SpacePoint;
}
  
/** Interface ID for ITrackFinder*/  
static const InterfaceID IID_ITrackFinder("ITrackFinder", 1, 0);
  
/**@class ITrackFinder

Abstract base class for TrackFinder AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ITrackFinder: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ITrackFinder(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITrackFinder; }

    /**ITrackFinder interface:
        deprecated - will no longer be needed with configurables */
    virtual void		setProperties (double                   maxSecondaryImpact,
                                               double                   minPt,
                                               int                      printLevel,
                                               const VertexRegion&      vertexRegion) = 0;

    /**ITrackFinder interface:
       perform the full iPatRec pattern recognition and fitting */
    virtual std::list<Track*>*	trackSearch (const std::vector<Trk::SpacePoint*>&) = 0;

};


#endif // IPATINTERFACES_ITRACKFINDER_H

