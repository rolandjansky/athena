/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_Recon.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRTRECON_H
# define IPATINTERFACES_ITRTRECON_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

/** Interface ID for ITRT_Recon*/  
static const InterfaceID IID_ITRT_Recon("ITRT_Recon", 1, 0);
  
/**@class ITRT_Recon

Abstract base class for TRT_Recon AlgTool
@performs TRT data access and pattern recognition for a single Track
     
@author Alan.Poppleton@cern.ch
*/

class HitOnTrack;
class PerigeeParameters;
class TrackParameters;

class ITRT_Recon: virtual public IAlgTool
{
public:
    
    /**Virtual destructor*/
    virtual ~ITRT_Recon(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITRT_Recon; }

    /**ITRT_Recon interface: bremLike
        current track has significant curvature increase */
    virtual bool		bremLike (void) const = 0;

    /**ITRT_Recon interface: reconstructSegment
	@null pointer   - outside TRT acceptance (or TRT deselected)
	@empty list     - TRT veto (i.e. no TRT association)
	@populated list - TRT confirmation (at least minStraws associated)*/
    virtual const std::vector<HitOnTrack*>*
                                reconstructSegment(const PerigeeParameters&	perigee,
						  const TrackParameters&	params,
						  const Trk::TrackParameters&	trkParameters) = 0;
};   

#endif // IPATINTERFACES_ITRTRECON_H
