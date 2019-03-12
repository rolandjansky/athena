/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolenoidalIntersector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXSOLENOIDALINTERSECTOR_SOLENOIDALINTERSECTOR_H
#define TRKEXSOLENOIDALINTERSECTOR_SOLENOIDALINTERSECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkExSolenoidalIntersector/SolenoidParametrization.h"
#include "CxxUtils/checker_macros.h"
#include <mutex>
#include <cmath>

class IIncidentSvc;
namespace Trk
{

class SolenoidalIntersector: public extends<AthAlgTool, IIntersector>
{
    
public:
    /**
     * @brief Constants of motion and other cached values.
     *
     * There is some data we want to persist across calls to intersectSurface()
     * for a single trajectory.  We do this by attaching this structure
     * as a cache block to TrackSurfaceIntersection.  Some of these are
     * actually constants of motion for a helical trajectory.  We also
     * include here the solenoidal parametrization.  This gets updated
     * as we step through the field; including it here avoids a reevaluation
     * at the start of the next call.  We also include the position at the
     * end of the stepping phase, m_lastPosition.  This is because before
     * after we finish stepping but before we return, we improve the
     * estimate of the intersection by calculating the straight line
     * intersection with the surface.  But if we then calcuate a further
     * intersection with the same trajectory, we want to start at the
     * point at the end of the previous stepping, not the position
     * we returned.
     */
    struct Constants
      : public TrackSurfaceIntersection::IIntersectionCache
    {
      Constants (const SolenoidParametrization& solpar,
                 const TrackSurfaceIntersection& trackTrackSurfaceIntersection,
                 const double qOverP);
      virtual std::unique_ptr<IIntersectionCache> clone() const override
      { return std::make_unique<Constants> (*this); }

      double m_sinTheta;
      double m_oneOverSinTheta;
      double m_cotTheta;
      double m_qOverPt;
      const SolenoidParametrization& m_solPar;
      Amg::Vector3D m_lastPosition;
      SolenoidParametrization::Parameters m_solParams;
    };
    SolenoidalIntersector	(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    virtual ~SolenoidalIntersector() = default;

    virtual StatusCode		initialize() override;
    virtual StatusCode		finalize() override;

    /**IIntersector interface method for general Surface type */
    virtual
    const TrackSurfaceIntersection*		intersectSurface(const Surface&		surface,
						 const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						 const double      	qOverP) const override;
	                                     
    /**IIntersector interface method for specific Surface type : PerigeeSurface */
    virtual
    const TrackSurfaceIntersection*		approachPerigeeSurface(const PerigeeSurface&	surface,
						       const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						       const double      	qOverP) const override;
	
    /**IIntersector interface method for specific Surface type : StraightLineSurface */
    virtual
    const TrackSurfaceIntersection*		approachStraightLineSurface(const StraightLineSurface& surface,
							    const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
							    const double      	qOverP) const override;
              
    /**IIntersector interface method for specific Surface type : CylinderSurface */
    virtual
    const TrackSurfaceIntersection*		intersectCylinderSurface (const CylinderSurface& surface,
							  const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
							  const double      	qOverP) const override;

    /**IIntersector interface method for specific Surface type : DiscSurface */
    virtual
    const TrackSurfaceIntersection*		intersectDiscSurface (const DiscSurface&	surface,
						      const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						      const double      	qOverP) const override;

    /**IIntersector interface method for specific Surface type : PlaneSurface */
    virtual
    const TrackSurfaceIntersection*		intersectPlaneSurface(const PlaneSurface&	surface,
						      const TrackSurfaceIntersection*	trackTrackSurfaceIntersection,
						      const double      	qOverP) const override;

    /**IIntersector interface method to check validity of parametrization within extrapolation range */
    virtual
    bool			isValid (Amg::Vector3D startPosition,
					 Amg::Vector3D endPosition) const override;

    /** tabulate parametrization details */
    virtual void		validationAction() const override;
    
private:
    const SolenoidParametrization*              getSolenoidParametrization() const;

    double					circularArcLength(double, double, double, double, double,
								    double, double&, double&) const;
    double					linearArcLength(const TrackSurfaceIntersection& isect,
                                                                const Constants& com,
                                                                const double radius2,
                                                                const double endRadius) const;
    bool					extrapolateToR(TrackSurfaceIntersection& isect,
                                                               double& radius2,
                                                               Constants&   com,
                                                               const double endRadius) const;
    bool					extrapolateToZ(TrackSurfaceIntersection& isect,
                                                               Constants&   com,
                                                               const double endZ) const;
    const TrackSurfaceIntersection*		intersection(std::unique_ptr<TrackSurfaceIntersection> isect,
                                                             Constants&     com,
                                                             const Surface& surface) const;

   std::unique_ptr<TrackSurfaceIntersection>
   newIntersection (const TrackSurfaceIntersection& oldIsect,
                    const SolenoidParametrization& solpar,
                    const double qOverP,
                    Constants*& com) const;

    ServiceHandle<MagField::IMagFieldSvc>	m_magFieldSvc;
    ToolHandle<IIntersector>			m_rungeKuttaIntersector;

    double					m_deltaPhiTolerance;
    double					m_surfaceTolerance;

    // counters
    mutable std::atomic<unsigned long long>	m_countExtrapolations;
    mutable std::atomic<unsigned long long>	m_countRKSwitches;

    mutable std::mutex m_mutex;
    mutable Gaudi::Hive::ContextSpecificPtr<const SolenoidParametrization> m_lastSolenoidParametrization ATLAS_THREAD_SAFE;
    // List of active solenoid parametrizations.  Second element of the pair
    // is a use count.
    typedef std::list<std::pair<SolenoidParametrization, int> > Parmlist_t;
    mutable Parmlist_t m_solenoidParametrizations ATLAS_THREAD_SAFE;
};


// arc length to intersect of 2 circles: circular track and circle at (0,0) with radius endRadius
inline double
SolenoidalIntersector::circularArcLength(double 	endRadius,
					 double		radiusOfCurvature,
					 double		xCentre,
					 double		yCentre,
					 double		cosPhi,
					 double		sinPhi,
					 double&	cosPhiIntersect,
					 double&	sinPhiIntersect) const
{
    int		trapped	= 0;
    double	radiusSquared 	= xCentre*xCentre + yCentre*yCentre;
    double	term 		= 0.5*(radiusSquared +
				       radiusOfCurvature*radiusOfCurvature -
				       endRadius*endRadius)/(radiusSquared*radiusOfCurvature);
    if (std::abs(xCentre) < std::abs(yCentre))
    {
	double	dx2 = yCentre*yCentre * (1./(radiusSquared*term*term) - 1.);
	if (dx2 < 0.)
	{
	    trapped = 1;
	}
	else
	{
	    if (yCentre*term > 0.)
	    {
		sinPhiIntersect = term*(-xCentre+std::sqrt(dx2));
	    }
	    else
	    {
		sinPhiIntersect = term*(-xCentre-std::sqrt(dx2));
	    }
	    cosPhiIntersect	= (sinPhiIntersect*xCentre + radiusSquared*term)/yCentre;
	}
    }
    else
    {
	double	dy2 = xCentre*xCentre * (1./(radiusSquared*term*term) - 1.);
	if (dy2 < 0.)
	{
	    trapped = 1;
	}
	else
	{
	    if (xCentre*term > 0.)
	    {
		cosPhiIntersect = term*(yCentre+std::sqrt(dy2));
	    }
	    else
	    {
		cosPhiIntersect = term*(yCentre-std::sqrt(dy2));
	    }
	    sinPhiIntersect	= (cosPhiIntersect*yCentre - radiusSquared*term)/xCentre;
	}
    }
    if (trapped == 0)
    {
	double	deltaPhi;
	double	sinDeltaPhi	= sinPhiIntersect*cosPhi - cosPhiIntersect*sinPhi;
	if (std::abs(sinDeltaPhi) > 0.1)
	{
	    deltaPhi = asin(sinDeltaPhi);
	}
	else
	{
	    deltaPhi = sinDeltaPhi*(1. + 0.166667*sinDeltaPhi*sinDeltaPhi);
	}
	return (radiusOfCurvature*deltaPhi);
    }
    else
    {
	cosPhiIntersect	= cosPhi;
	sinPhiIntersect = sinPhi;
	return 0.;
    }
}    
	    
// arc length to intersect of a line to a circle of radius endRadius centred at (0,0)
// +ve (-ve) endRadius selects the solution on the same (opposite) side of (0,0)
inline double
SolenoidalIntersector::linearArcLength(const TrackSurfaceIntersection& isect,
                                       const Constants& com,
                                       const double	radius2,
                                       const double	endRadius) const
{
    const Amg::Vector3D& pos = isect.position();
    const Amg::Vector3D& dir = isect.direction();

    double 	arcLength     	= (-dir.x()*pos.x() - dir.y()*pos.y()) *
				  com.m_oneOverSinTheta;
    double 	radiusSquared	= endRadius*endRadius - radius2 + arcLength*arcLength;
    if (radiusSquared > 0.)
    {
	if (endRadius > 0.)
	{
	    arcLength += std::sqrt(radiusSquared);
	}
	else
	{
	    arcLength -= std::sqrt(radiusSquared);
	}
    }
    return arcLength;
}
    
inline const TrackSurfaceIntersection*
SolenoidalIntersector::intersection(std::unique_ptr<TrackSurfaceIntersection> isect,
                                    Constants&          com,
                                    const Surface&	surface) const
{
    // Improve the estimate of the intersection by calculating
    // the straight-line intersection.
    Intersection SLIntersect	= surface.straightLineIntersection(isect->position(), isect->direction(), false, false);
    if (SLIntersect.valid)
    {
        // But first save our current position, so that we can
        // start from here on the next call.
        com.m_lastPosition = isect->position();
        isect->position() = SLIntersect.position;
	return isect.release();
    }

    return nullptr;
}

} // end of namespace


#endif // TRKEXSOLENOIDALINTERSECTOR_SOLENOIDALINTERSECTOR_H

