/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Trackstateonsurfacecomparisonfunction.h
//   Header file for struct TrackstateonSurfaceComparisonFunction
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch / Andreas.Salzburger@cern.ch  / Martin.Siebel@CERN.ch
///////////////////////////////////////////////////////////////////


#ifndef TRKNIRVANA_TSOSCOMPARISONFUNCTION_H
#define TRKNIRVANA_TSOSCOMPARISONFUNCTION_H

//Trk
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
// extra-maths for cylinder intersections
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "GeoPrimitives/GeoPrimitives.h"
//STL
#include <ext/algorithm>

namespace Trk {

  /** Class inheriting from std::binary_function to provide a
   *     comparison function, or relational definition, for
   *     sorting MeasurementBase objects
   */
  class TrackStateOnSurfaceComparisonFunction 
    : public std::binary_function<const Trk::MeasurementBase*,
                                  const Trk::MeasurementBase*, bool> {
  public:

    /** Default Constructor, default will be sorting towards radius of 0 */
    TrackStateOnSurfaceComparisonFunction()                
      : m_point(0),
          m_direction(0),
          m_radius(0.)
          {}

    /** Default Constructor, using a given radius */
    TrackStateOnSurfaceComparisonFunction(double cradius)
      : m_point(0),
          m_direction(0),
          m_radius(fabs(cradius))
          {}
    
    // /** Simple relation definition using a 3d distance to the reference point */
    // TrackStateOnSurfaceComparisonFunction(const Amg::Vector3D& sp)
    //   : m_point(new Amg::Vector3D(sp)),
    //       m_direction(0),
    //       m_radius(0.)
    //       {}

    /** Simple relation definition using a 3d distance to the reference point */
    TrackStateOnSurfaceComparisonFunction(const Amg::Vector3D& dir)
      : m_point(new Amg::Vector3D(0.,0.,0.)),
          m_direction(new Amg::Vector3D(dir.unit())),
          m_radius(0.)
          {}

    /** Full relation definition using a straight line propagation */
    TrackStateOnSurfaceComparisonFunction(const Amg::Vector3D& sp,
                                      const Amg::Vector3D&     dir)
      : m_point(new Amg::Vector3D(sp)),
          m_direction(new  Amg::Vector3D(dir.unit())),
          m_radius(0.)
      {}

    /** Copy Ctor */
    TrackStateOnSurfaceComparisonFunction(const TrackStateOnSurfaceComparisonFunction& MCF)
      :  m_point(MCF.m_point ? new Amg::Vector3D(*MCF.m_point) : 0),
          m_direction(MCF.m_direction  ? new Amg::Vector3D(*MCF.m_direction) : 0),
          m_radius(MCF.m_radius)
      {}

    /** Destructor */
    virtual ~TrackStateOnSurfaceComparisonFunction(){
      delete m_point;
      delete m_direction;
    }

    TrackStateOnSurfaceComparisonFunction &operator=(const TrackStateOnSurfaceComparisonFunction& MCF)
    {
      if (this != &MCF) {
        delete m_point;
        m_point = (MCF.m_point ? new Amg::Vector3D(*MCF.m_point) : 0);
        delete m_direction;
        m_direction=(MCF.m_direction  ? new Amg::Vector3D(*MCF.m_direction) : 0);
        m_radius=(MCF.m_radius);
      }
      return *this;
    }
               
    /** The comparison function defining in what case a PRD is 'smaller' than
        a second one */
    bool operator() (const Trk::TrackStateOnSurface* one,
		     const Trk::TrackStateOnSurface* two) const {

      Amg::Vector3D gp1;
      Amg::Vector3D gp2;
      const Trk::Surface*  surf1 = 0;
      const Trk::Surface*  surf2 = 0;


      if ( ! ( one->trackParameters() || one->measurementOnTrack() ) )
	{
          std::cout << "TrackStateOnSurfaceComparisonFunction: input TSOS one not sufficient" << std::endl;
	  return true;
	}
      if ( ! ( two->trackParameters() || two->measurementOnTrack() ) )
	{
          std::cout << "TrackStateOnSurfaceComparisonFunction: input TSOS two not sufficient" << std::endl;
	  return false;
	}

      if (one->trackParameters())
	{
	  gp1 = one->trackParameters()->position();
	  surf1 = &one->trackParameters()->associatedSurface();	  
	  if (!surf1 && one->measurementOnTrack() )
	    surf1 = &(one->measurementOnTrack()->associatedSurface());	  
	}
      else if (one->measurementOnTrack())
	{
	  gp1 = one->measurementOnTrack()->globalPosition();
	  surf1 = &(one->measurementOnTrack()->associatedSurface());	  
	}

      if (two->trackParameters())
	{
	  gp2 = two->trackParameters()->position();
	  surf2 = &two->trackParameters()->associatedSurface();	  
	  if (!surf2 && two->measurementOnTrack() )
	    surf2 = &(two->measurementOnTrack()->associatedSurface());	  
	}
      else if (two->measurementOnTrack())
	{
	  gp2 = two->measurementOnTrack()->globalPosition();
	  surf2 = &(two->measurementOnTrack()->associatedSurface());	  
	}

      

      // --- very simple case, check radial distances
      if (!m_direction && !m_point) {
        return ( fabs(gp1.perp() - m_radius)
                 < fabs(gp2.perp() - m_radius) );
      }
      // --- simple case, just use global position distances
      else if (!m_direction) {
        return ( (gp1 - *m_point).mag()
                 < (gp2 - *m_point).mag());
      }
      // --- flexible sorting along a predicted direction
      else {
	if ( ! ( surf1 && surf2 ) )
	  {
	    std::cout << "TrackStateOnSurfaceComparisonFunction: surface missing" << std::endl;
	    return false;
	  }

        double path1 = 0;
        //const Trk::Surface&              sf1 = one->associatedSurface();
        const Trk::PlaneSurface*        opsf = dynamic_cast <const Trk::PlaneSurface*>(surf1);
        const Trk::StraightLineSurface* ossf = 0;
        const Trk::DiscSurface*         odsf = 0; // } dyncast only when necessary
        const Trk::CylinderSurface*     ocsf = 0;
        const Trk::PerigeeSurface*      ogsf = 0;
        if (opsf) {
          path1 = this->pathIntersectWithPlane(*opsf);
        } else if ((ossf = dynamic_cast< const Trk::StraightLineSurface* >(surf1)) && ossf) {
          path1 = this->pathIntersectWithLine (*ossf);
        } else if ((odsf = dynamic_cast <const Trk::DiscSurface*> (surf1)) && odsf) {
          path1 = this->pathIntersectWithDisc (*odsf);
        } else if ((ocsf = dynamic_cast <const Trk::CylinderSurface*> (surf1)) && ocsf) {
          path1 = this->pathIntersectWithCylinder(*ocsf, gp1);
        } else if ((ogsf = dynamic_cast< const Trk::PerigeeSurface* >(surf1)) && ogsf) {
          path1 = this->pathIntersectWithLine (*ogsf);
        }else {
          std::cout << "TrackStateOnSurfaceComparisonFunction: surface type error for Sf1!" << std::endl;
        }

        // --- identify the 2nd surface type and get intersection path for surface 1
        //
        double path2 = 0;
        //const Trk::Surface&              sf2 = two->associatedSurface();
        const Trk::PlaneSurface*        tpsf = dynamic_cast< const Trk::PlaneSurface* >(surf2);
        const Trk::StraightLineSurface* tssf = 0;
        const Trk::DiscSurface*         tdsf = 0; // dyncast only when necessary
        const Trk::CylinderSurface*     tcsf = 0;
        const Trk::PerigeeSurface*      tgsf = 0;
        if (tpsf) {
          path2 = this->pathIntersectWithPlane(*tpsf);
        } else if ((tssf = dynamic_cast< const Trk::StraightLineSurface* >(surf2)) && tssf) {
          path2 = this->pathIntersectWithLine (*tssf);
        } else if ((tdsf = dynamic_cast< const Trk::DiscSurface* > (surf2)) && tdsf) {
          path2 = this->pathIntersectWithDisc (*tdsf);
        } else if ((tcsf = dynamic_cast <const Trk::CylinderSurface*> (surf2)) && tcsf) {
          path2 = this->pathIntersectWithCylinder(*tcsf, gp2);
        } else if ((tgsf = dynamic_cast< const Trk::PerigeeSurface* >(surf2)) && tgsf) {
          path2 = this->pathIntersectWithLine (*tgsf);
        } else {
          std::cout << "TrackStateOnSurfaceComparisonFunction: surface type error for Sf2!" << std::endl;
        }
        
        return path1 < path2;

      }
    }
  private:
    Amg::Vector3D* m_point;
    Amg::Vector3D*     m_direction;
    double          m_radius;

    double pathIntersectWithPlane(const Trk::PlaneSurface& psf) const
    {
      double denom = m_direction->dot(psf.normal()); // c++ can be unreadable
      return denom ?
        psf.normal().dot(psf.center() - *m_point)/denom :
        denom                                            ;
    }

    double pathIntersectWithLine(const Trk::StraightLineSurface& lsf) const
    {
      Amg::Vector3D dirWire(lsf.transform().rotation().col(2).unit());
      Amg::Vector3D trackToWire(lsf.center() - *m_point);
      double     parallelity = m_direction->dot(dirWire);
      double     denom       = 1 - parallelity*parallelity;
      return (fabs(denom)>10e-7)                       ?
        (trackToWire.dot(*m_direction) 
         - trackToWire.dot(dirWire)*parallelity)/denom :
        0.                                             ;
    }

    double pathIntersectWithLine(const Trk::PerigeeSurface& pgsf) const
    {
      Amg::Vector3D trackToWire(pgsf.center() - *m_point);
      double     parallelity = m_direction->dot(Trk::s_zAxis);
      double     denom       = 1 - parallelity*parallelity;
      return (fabs(denom)>10e-7)                            ?
        (trackToWire.dot(*m_direction) 
         - trackToWire.dot(Trk::s_zAxis)*parallelity)/denom :
        0.                                                  ;
    }

    double pathIntersectWithDisc(const Trk::DiscSurface& dsf) const
    {
      double denom = m_direction->dot(dsf.normal());
      return denom                                     ?
        dsf.normal().dot(dsf.center() - *m_point)/denom :
        denom                                            ;
    }

    double pathIntersectWithCylinder(const Trk::CylinderSurface& csf,
                                     const Amg::Vector3D&  globalHit) const
    { // --- code from TrkExSlPropagator/LineCylinderIntersection.cxx
     
      // get the rotation by reference
      const Amg::Transform3D& locTrans = csf.transform();
      // take two points of line and calculate them to the 3D frame of the cylinder
      Amg::Vector3D point1(locTrans.inverse() * * m_point);
      Amg::Vector3D point2raw = * m_point + * m_direction;
      Amg::Vector3D point2(locTrans.inverse() * point2raw); // do it in two steps - CLHEP stability

      // new direction in 3D frame of cylinder
      Amg::Vector3D direc((point2 - point1).unit());

      if (!direc.x()){
        return 0.;
      } else {
        // get line and circle constants
        double k = (direc.y())/(direc.x());
        double d = (point2.x()*point1.y() - point1.x()*point2.y())/(point2.x() - point1.x());
        double R = csf.bounds().r();
        double first = 0.;
        double second= 0.;
    
        // and solve the quadratic equation  Trk::RealQuadraticEquation pquad(1+k*k, 2*k*d, d*d-R*R);
        double a = 1 + k*k;
        double p = 2*k*d;
        double q = d*d - R*R;
        double discriminant = p*p - 4*a*q;
        if (discriminant<0) {
          return 0.;
        } else {
          //          solutions = (discriminant==0) ? one : two;
          double x0 = -0.5*(p + (p>0 ? sqrt(discriminant) : -sqrt(discriminant)));
          first = x0/a;
          second = q/x0;
        }
        double t1 = (first  - point1.x())/direc.x();
        double t2 = (second - point1.x())/direc.x();
        // the solutions in the 3D frame of the cylinder
        Amg::Vector3D dist1raw(point1 + t1 * direc - globalHit);
        Amg::Vector3D dist2raw(point1 + t2 * direc - globalHit);
        // return the solution which is closer to Meas'Base's global coordinates
        if ( dist1raw.mag() < dist2raw.mag() ) {
          return t1; // FIXME - wrong line parameterisation
        } else {
          return t2;
        }
      }
    }
  };

} // end of namespace

#endif //TRKNIRVANA_TSOSCOMPARISONFUNCTION_H

