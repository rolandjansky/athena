/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataComparisonFunction.h
//   Header file for struct PrepRawDataComparisonFunction
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch / Andreas.Salzburger@cern.ch
///////////////////////////////////////////////////////////////////


#ifndef TRKNIRVANA_PREPRAWDATACOMPARISONFUNCTION_H
#define TRKNIRVANA_PREPRAWDATACOMPARISONFUNCTION_H

//Trk
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "GeoPrimitives/GeoPrimitives.h"
//STL
#include <ext/algorithm>

namespace Trk {

  /** Class inheriting from std::binary_function to provide a
    * comparison function, or relational definition, for PrepRawData
    */
  class PrepRawDataComparisonFunction :
    public std::binary_function<const Trk::PrepRawData*, const Trk::PrepRawData*, bool>
  {
  public:
                
    /** Default Constructor */
    PrepRawDataComparisonFunction()
      : m_point(new Amg::Vector3D(0., 0., 0.)),
      m_direction(0)
      {}

    /** Simple relation definition using a 3d distance to the reference point */
    PrepRawDataComparisonFunction(const Amg::Vector3D& sp)
      : m_point(new Amg::Vector3D(sp)),
      m_direction(0)
      {}

    /** Full relation definition using a straight line propagation */
    PrepRawDataComparisonFunction(const Amg::Vector3D& sp,
                                  const Amg::Vector3D&     dir)
      : m_point(new Amg::Vector3D(sp)),
      m_direction(new  Amg::Vector3D(dir.unit()))
      {}

    /** Copy Ctor */
    PrepRawDataComparisonFunction(const PrepRawDataComparisonFunction& PCF)
      :  m_point(PCF.m_point ? new Amg::Vector3D(*PCF.m_point) : 0),
      m_direction(PCF.m_direction  ? new Amg::Vector3D(*PCF.m_direction) : 0)
      {}

    /** Destructor */
    virtual ~PrepRawDataComparisonFunction(){
      delete m_point;
      delete m_direction;
    }

    PrepRawDataComparisonFunction &operator=(const PrepRawDataComparisonFunction& PCF) {
      if (this != &PCF) {
        delete m_point;
        m_point=(PCF.m_point ? new Amg::Vector3D(*PCF.m_point) : 0);
        delete m_direction;
        m_direction=(PCF.m_direction  ? new Amg::Vector3D(*PCF.m_direction) : 0); 
      }
      return *this;
    }

    /** The comparison function defining in what case a PRD is 'smaller' than
        a second one */
    bool operator() (const Trk::PrepRawData* one,
                     const Trk::PrepRawData* two) const {

      if (!m_direction) { // simple case, just return surface distances
        return ( (one->detectorElement()->surface( one->identify() ).center()
                  - *m_point).mag()
                 < (two->detectorElement()->surface( two->identify() ).center()
                    - *m_point).mag()
               );
      } else {
        // --- identify the surface type and get intersection path for surface 1
        //
        double path1 = 0;
        const Trk::Surface&              sf1 = one->detectorElement()->surface( one->identify() );
        const Trk::PlaneSurface*        opsf = dynamic_cast <const Trk::PlaneSurface*>(&sf1);
        const Trk::StraightLineSurface* ossf = 0; // dyncast only when necessary
        const Trk::DiscSurface*         odsf = 0;
        if (opsf) {
          path1 = this->pathIntersectWithPlane(*opsf);
        } else if ((ossf = dynamic_cast< const Trk::StraightLineSurface* >(&sf1)) && ossf) {
          path1 = this->pathIntersectWithLine (*ossf);
        } else if ((odsf = dynamic_cast <const Trk::DiscSurface*> (&sf1)) && odsf) {
          path1 = this->pathIntersectWithDisc (*odsf);
        } else {
          std::cout << "PrepRawDataComparisonFunction: surface type error!" << std::endl;
        } // --- no raw data on Cylinder. Ever.
        
        // --- identify the surface type and get intersection path for surface 1
        //
        double path2 = 0;
        const Trk::Surface&              sf2 = two->detectorElement()->surface( two->identify() );
        const Trk::PlaneSurface*        tpsf = dynamic_cast< const Trk::PlaneSurface* >(&sf2);
        const Trk::StraightLineSurface* tssf = 0; // dyncast only when necessary
        const Trk::DiscSurface*         tdsf = 0;
        if (tpsf) {
          path2 = this->pathIntersectWithPlane(*tpsf);
        } else if ((tssf = dynamic_cast< const Trk::StraightLineSurface* >(&sf2)) && tssf) {
          path2 = this->pathIntersectWithLine (*tssf);
        } else if ((tdsf = dynamic_cast< const Trk::DiscSurface* > (&sf2)) && tdsf) {
          path2 = this->pathIntersectWithDisc (*tdsf);
        } else {
          std::cout << "PrepRawDataComparisonFunction: surface type error!" << std::endl;
        } // --- no PRD on Cylinder. Ever.
        
        return path1 < path2;
      }

    }

  private:
    Amg::Vector3D* m_point;
    Amg::Vector3D*     m_direction;

    double pathIntersectWithPlane(const Trk::PlaneSurface& psf) const
    {
      double denom = m_direction->dot(psf.normal()); // c++ can be unreadable
      return (denom) ?
        psf.normal().dot(psf.center() - *m_point)/(denom) :
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

    double pathIntersectWithDisc(const Trk::DiscSurface& dsf) const
    {
      double denom = m_direction->dot(dsf.normal());
      return (denom)                                     ?
        dsf.normal().dot(dsf.center() - *m_point)/(denom) :
        denom                                            ;
    }
  };

} // end of namespace

#endif //TRKNIRVANA_PREPRAWDATACOMPARISONFUNCTION_H

