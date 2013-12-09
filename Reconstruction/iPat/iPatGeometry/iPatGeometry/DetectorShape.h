/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 abstract base class describing the shape of an InDet detector element
 ***************************************************************************/

#ifndef IPATGEOMETRY_DETECTORSHAPE_H
# define IPATGEOMETRY_DETECTORSHAPE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "iPatGeometry/DetectorIntersect.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class DetectorShape
{

public:
    // implicit constructor
    virtual ~DetectorShape (void) {};
    
    // implicit copy constructor
    // implicit assignment operator

    // crude acceptance check in eta direction
    virtual bool       		insideEta (double localEta) const = 0;
    
    // intersect subject to precise acceptance check using predicted errors
    virtual DetectorIntersect*	intersect(double localEta,
					  double localPhi,
					  double rzTol,
					  double rphiTol,
					  double absStereo) const = 0;
    // expose local widths
    virtual double		localEtaWidth (void) const = 0;
    virtual double      	localPhiWidth (void) const = 0;

    // debug print
    virtual void		print (void) const = 0;
    
    // comparison
    virtual bool		sameShape (const DetectorShape& shape) const = 0;
    
protected:
    // tolerance used for crude checks around detector boundary
    static const double		s_boundaryTolerance;
    // tolerance used for shape comparison
    static const double		s_shapeTolerance;
};

#endif // IPATGEOMETRY_DETECTORSHAPE_H

