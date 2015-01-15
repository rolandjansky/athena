/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasBaseIndexComparisonFunction.h
//   Header file for struct MeasBaseIndexComparisonFunction
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRKFITTER_MEASBASEINDEXCOMPARISONFUNCTION_H
#define TRKFITTER_MEASBASEINDEXCOMPARISONFUNCTION_H

//Trk
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
//STL
#include <ext/algorithm>

namespace Trk {

  typedef std::pair<const Trk::MeasurementBase*, int> MeasBaseIndex;

  /** @brief class inheriting from std::binary_function to provide
   a comparison function, or relational definition, for sorting
   MeasurementBase objects in pair with an index.

   This index can be used to keep a link to the original position index
   e.g. on a track. As with the objects in TrkEventUtils, the ordering
   can be made with respect to a point, direction or straight
   line vector.

   @author Wolfgang.Liebig <http://consult.cern.ch/xwho/people/54608>

   */
  class MeasBaseIndexComparisonFunction 
    : public std::binary_function<MeasBaseIndex,
                                  MeasBaseIndex, bool> {
  public:

    //! Default Constructor, default will be sorting towards radius of 0
    MeasBaseIndexComparisonFunction()                
      : m_point(0),
          m_direction(0),
          m_radius(0.)
          {}

    //! Default Constructor, using a given radius
    MeasBaseIndexComparisonFunction(double cradius)
      : m_point(0),
          m_direction(0),
          m_radius(fabs(cradius))
          {}
    
    //! Simple relation definition using a 3d distance to the reference point
    MeasBaseIndexComparisonFunction(const Amg::Vector3D& sp)
      : m_point(new Amg::Vector3D(sp)),
          m_direction(0),
          m_radius(0.)
          {}

    //! Full relation definition using a straight line propagation
    MeasBaseIndexComparisonFunction(const Amg::Vector3D& sp,
                                    const Amg::Vector3D&     dir)
      : m_point(new Amg::Vector3D(sp)),
        m_direction(new  Amg::Vector3D(dir.unit())),
        m_radius(0.)
      {}

    //! Copy Ctor
    MeasBaseIndexComparisonFunction(const MeasBaseIndexComparisonFunction& MCF)
      :  m_point(MCF.m_point ? new Amg::Vector3D(*MCF.m_point) : 0),
          m_direction(MCF.m_direction  ? new Amg::Vector3D(*MCF.m_direction) : 0),
          m_radius(MCF.m_radius)
      {}

    //! Destructor
    virtual ~MeasBaseIndexComparisonFunction(){
      delete m_point;
      delete m_direction;
    }

    MeasBaseIndexComparisonFunction &operator=(const MeasBaseIndexComparisonFunction& MCF) {
      if (&MCF != this ) {
        delete m_point;
        delete m_direction;
        m_point = (MCF.m_point ? new Amg::Vector3D(*MCF.m_point) : 0);
        m_direction = (MCF.m_direction  ? new Amg::Vector3D(*MCF.m_direction) : 0);
        m_radius = (MCF.m_radius);
      }
      return *this;
    }
    
               
    /** @brief The comparison function defining in what case one hit
        is 'smaller' than a second one */
    bool operator() (MeasBaseIndex one,
                     MeasBaseIndex two) const {

      // --- very simple case, check radial distances
      if (!m_direction && !m_point) {
        return ( fabs(one.first->globalPosition().perp() - m_radius)
                 < fabs(two.first->globalPosition().perp() - m_radius) );
      }
      // --- simple case, just use global position distances
      else if (!m_direction) {
        return ( (one.first->globalPosition() - *m_point).mag()
                 < (two.first->globalPosition() - *m_point).mag());
      }
      // --- check if hit distance points into same hemisphere as reference dir
      else {
        const Trk::PlaneSurface* opsf = 
          dynamic_cast <const Trk::PlaneSurface*>(&one.first->associatedSurface());
        const Trk::PlaneSurface* tpsf = 
          dynamic_cast< const Trk::PlaneSurface*>(&two.first->associatedSurface());
        if (opsf && tpsf) {
          return (this->pathIntersectWithPlane(*opsf)
                  < this->pathIntersectWithPlane(*tpsf));
        } else {
          Amg::Vector3D t(one.first->globalPosition() - two.first->globalPosition());
          return m_direction->dot(t) <=0.0;
        }
      }
      return false;
    }

  private:
    Amg::Vector3D* m_point;
    Amg::Vector3D* m_direction;
    double         m_radius;

    double pathIntersectWithPlane(const Trk::PlaneSurface& psf) const
    {
      double denom = m_direction->dot(psf.normal());
      return (denom) ?
        psf.normal().dot(psf.center() - *m_point)/(denom) :
        denom                                            ;
    }

  };

} // end of namespace

#endif //TRKFITTER_MEASBASEINDEXCOMPARISONFUNCTION_H

