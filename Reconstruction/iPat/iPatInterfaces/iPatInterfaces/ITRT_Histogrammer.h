/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_Histogrammer.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRTHISTOGRAMMER_H
# define IPATINTERFACES_ITRTHISTOGRAMMER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

/** Interface ID for ITRT_Histogrammer*/  
static const InterfaceID IID_ITRT_Histogrammer("ITRT_Histogrammer", 1, 0);
  
/**@class ITRT_Histogrammer

Abstract base class for TRT_Histogrammer AlgTool
@performs TRT data access and pattern recognition for a single Track
     
@author Alan.Poppleton@cern.ch
*/

class TRT_RoadData;

class ITRT_Histogrammer: virtual public IAlgTool
{
public:
    
    /**Virtual destructor*/
    virtual ~ITRT_Histogrammer(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITRT_Histogrammer; }

    /**ITRT_Histogrammer interface: bremLike
        current track has significant curvature increase */
    virtual bool		bremLike (double rMax, double rPhiMax) const = 0;

    /**ITRT_Histogrammer interface: confirmSegment
	selects and confirms the RoadData objects forming a TRT track segment */
    virtual bool		confirmSegment (const Trk::TrackParameters&		trkParameters,
						const std::vector<TRT_RoadData*>& 	roadData) = 0;

    /**ITRT_Histogrammer interface: numLayers
        number of trt layers associated to current segment */
    virtual int			numLayers (void) const = 0;

    /**ITRT_Histogrammer interface: numTrHits
        number of high threshold hits on current segment */
    virtual int			numTrHits (void) const = 0;

    /**ITRT_Histogrammer interface: residual
        residual for a particular RoadData object */
    virtual double		residual (const TRT_RoadData&) const = 0;

    /**ITRT_Histogrammer interface: trRich
        current segment has a significant proportion of high threshold hits */
    virtual bool		trRich (void) const = 0;

};   

#endif // IPATINTERFACES_ITRTHISTOGRAMMER_H
