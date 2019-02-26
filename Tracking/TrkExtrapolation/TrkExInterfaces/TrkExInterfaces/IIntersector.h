/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IIntersector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IINTERSECTOR_H
#define TRKEXINTERFACES_IINTERSECTOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"


namespace Trk
{
    class CylinderSurface;
    class DiscSurface;
    class PerigeeSurface;
    class PlaneSurface;
    class StraightLineSurface;
    class Surface;
    class TrackIntersection;
  
    /**@class IIntersector

    Base class for Intersector AlgTools.
     
    Returning a NULL pointer means that the intersection/approach has not converged, 
    or there has not been a mathematical solution for this.
     
    @author Andreas.Salzburger@cern.ch

    modifications from Alan.Poppleton@cern.ch:
    - I've added a preliminary set of interfaces that are sufficient to get me going.
    - The TrackParameter interfaces are just a suggestion - there is no implementation.
    - I've not included PropDirection as the code discovers this by itself !
    
    */
    class IIntersector : virtual public IAlgTool
    {

    protected:
    
    public:
        DeclareInterfaceID(IInterface, 1, 0);

	/**Virtual destructor*/
	virtual ~IIntersector() = default;
       
	/**IIntersector interface method for general Surface type */
	virtual const TrackSurfaceIntersection* intersectSurface(const Surface&		surface,
						     const TrackSurfaceIntersection*	trackIntersection,
						     const double      		qOverP) const = 0;
               
	/**IIntersector interface method for specific Surface type : PerigeeSurface */
	virtual const TrackSurfaceIntersection* approachPerigeeSurface(const PerigeeSurface&	surface,
							   const TrackSurfaceIntersection*		trackIntersection,
							   const double      		qOverP) const = 0;
	
	/**IIntersector interface method for specific Surface type : StraightLineSurface */
	virtual const TrackSurfaceIntersection* approachStraightLineSurface(const StraightLineSurface& surface,
								const TrackSurfaceIntersection*	trackIntersection,
								const double      	qOverP) const = 0;
              
	/**IIntersector interface method for specific Surface type : CylinderSurface */
	virtual const TrackSurfaceIntersection* intersectCylinderSurface (const CylinderSurface&	surface,
							      const TrackSurfaceIntersection*	trackIntersection,
							      const double      	qOverP) const = 0;

	/**IIntersector interface method for specific Surface type : DiscSurface */
	virtual const TrackSurfaceIntersection* intersectDiscSurface (const DiscSurface&	surface,
							  const TrackSurfaceIntersection*	trackIntersection,
							  const double      	qOverP) const = 0;

	/**IIntersector interface method for specific Surface type : PlaneSurface */
	virtual const TrackSurfaceIntersection* intersectPlaneSurface(const PlaneSurface&	surface,
							  const TrackSurfaceIntersection*	trackIntersection,
							  const double      	qOverP) const = 0;
 
	/**IIntersector interface method
	   for extrapolation validity check over a particular extrapolation range */
	virtual bool		    isValid(Amg::Vector3D startPosition,
					    Amg::Vector3D endPosition) const = 0;
	
	/** Validation Action:
	    Can be implemented optionally, outside access to internal validation steps */
	virtual void validationAction() const
	    {}

    };

} // end of namespace


#endif // TRKEXINTERFACES_IINTERSECTOR_H


