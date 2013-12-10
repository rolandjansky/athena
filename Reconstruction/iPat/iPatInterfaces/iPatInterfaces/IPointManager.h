/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPointManager.h, (c) ATLAS iPatRec
//
// allocate SpacePoint's into a point_vector per 'idealized' layer.
// These are further sub-divided into projective eta-slices.
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_IPOINTMANAGER_H
# define IPATINTERFACES_IPOINTMANAGER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

/** Interface ID for IPointManager*/  
static const InterfaceID IID_IPointManager("IPointManager", 1, 0);
  
/**@class IPointManager

Abstract base class for PointManager AlgTool
      
@author Alan.Poppleton@cern.ch
*/

namespace Trk	{ class SpacePoint;	}

class Point;

class IPointManager: virtual public IAlgTool
{ 
public:
    /**Virtual destructor*/
    virtual ~IPointManager()	{}

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&		interfaceID() { return IID_IPointManager; }

    /**IPointManager interface: 	*/
    virtual void			clear (void) = 0;
    
    /**IPointManager interface: 	*/
    virtual void			makeEtaSlices (double vertex, double halfWidth) = 0;
    
    /**IPointManager interface: 	*/
    virtual void			newSearch (void) = 0;
    
    /**IPointManager interface: 	*/
    virtual bool			nextSlice (void) = 0;
    
    /**IPointManager interface: 	*/
    virtual std::vector<Point*>*	points (int layer) const = 0;
    
    /**IPointManager interface: 	*/
    virtual double			radius (int layer) const = 0;
    
    /**IPointManager interface: 	*/
    virtual void			setPoints (
	const std::vector<Trk::SpacePoint*>&	spacePoints,
	const Amg::Vector3D&			vertexRegionCentre) = 0;

};

#endif	// IPATINTERFACES_IPOINTMANAGER_H
