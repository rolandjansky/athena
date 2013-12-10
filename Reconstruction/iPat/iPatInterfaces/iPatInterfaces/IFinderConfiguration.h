/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFinderConfiguration.h, (c) ATLAS iPatRec
//
// centralised definition of all cuts and tolerances applied during
// iPatRec track finding.
//
// FIXME: to be extended to generically describe layout model and
// combinatorial search strategy.
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_IFINDERCONFIGURATION_H
# define IPATINTERFACES_IFINDERCONFIGURATION_H

#include "GaudiKernel/IAlgTool.h"

/** Interface ID for IFinderConfiguration*/  
static const InterfaceID IID_IFinderConfiguration("IFinderConfiguration", 1, 0);
  
/**@class IFinderConfiguration

Abstract base class for FinderConfiguration AlgTool
      
@author Alan.Poppleton@cern.ch
*/

class VertexRegion;

class IFinderConfiguration: virtual public IAlgTool
{
public:
    /**Virtual destructor*/
    virtual ~IFinderConfiguration()	{}
    
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&		interfaceID() { return IID_IFinderConfiguration; }

    /**IFinderConfiguration interface:    
       halfField clients: CombinationMaker, PrimaryCandidate, SecondaryCandidate	*/
    virtual double			halfField (void) const = 0;

    /**IFinderConfiguration interface: 	
       maxCurvature clients: CombinationMaker, PointManager	*/
    virtual double			maxCurvature (void) const = 0;
    virtual double			maxCurvature (double minPt) const = 0;

    /**IFinderConfiguration interface: 	
       maxDeltaRZ client: SecondaryCandidate	*/
    virtual double			maxDeltaRZ (void) const = 0;

    /**IFinderConfiguration interface: 	
       maxDeltaEta/Phi client: PrimaryCandidate	*/
    virtual double			maxPrimaryDeltaEtaPix (void) const = 0;
    virtual double			maxPrimaryDeltaEtaSct (void) const = 0;
    virtual double			maxPrimaryDeltaEtaVtx (void) const = 0;
    virtual double			maxPrimaryDeltaPhiPix (void) const = 0;
    virtual double			maxPrimaryDeltaPhiSct (void) const = 0;
    virtual double			maxPrimaryDeltaPhiVtx (void) const = 0;

    /**IFinderConfiguration interface: 	
       maxPhiSlope clients: PrimaryCandidate, SecondaryCandidate	*/
    virtual double			maxPhiSlope (void) const = 0;

    /**IFinderConfiguration interface:    
       maxPrimaryImpact clients: CombinationMaker	*/
    virtual double			maxPrimaryImpact (void) const = 0;

    /**IFinderConfiguration interface:    
       maxSecondaryImpact clients: CombinationMaker, SecondaryCandidate	*/
    virtual double			maxSecondaryImpact (void) const = 0;

    /**IFinderConfiguration interface: 
       range of vertexZ clients: PrimaryCandidate, SecondaryCandidate, TrackFinder	*/
    virtual double			maxVertexZ (void) const = 0;
    virtual double			minVertexZ (void) const = 0;

    /**IFinderConfiguration interface:
       minPt clients: CombinationMaker, TrackFinder, TrackManager	*/
    virtual double			minPt (void) const = 0;
    virtual void			minPt (double value) = 0;

    /**IFinderConfiguration interface: 	*/
    virtual int				printLevel (void) const = 0;
    virtual void			setProperties (double			maxSecondaryImpact,
						       double			minPt,
						       int			printLevel,
						       const VertexRegion&	vertexRegion) = 0;

    /**IFinderConfiguration interface: 	*/
    virtual void			vertexZLimits (void) = 0;
    virtual void			vertexZLimits (double z) = 0;
    virtual void			vertexZLimits (double zMax, double zMin) = 0;
};

#endif	// IPATINTERFACES_IFINDERCONFIGURATION_H
