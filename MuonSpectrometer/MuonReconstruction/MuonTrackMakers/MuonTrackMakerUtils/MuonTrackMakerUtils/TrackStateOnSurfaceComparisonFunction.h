/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackStateOnSurfaceComparisonFunction.h
//   Header file for struct TrackStateOnSurfaceComparisonFunction
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch / Andreas.Salzburger@cern.ch / Martin.Siebel@CERN.ch
///////////////////////////////////////////////////////////////////


#ifndef MUONTGRECTOOLS_TRACKSTATEONSURFACECOMPARISONFUNCTION_H
#define MUONTGRECTOOLS_TRACKSTATEONSURFACECOMPARISONFUNCTION_H

//Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
// extra-maths for cylinder intersections
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkParameters/TrackParameters.h"
//STL
#include <ext/algorithm>

namespace Muon {

  /** Class inheriting from std::binary_function to provide a
   *     comparison function, or relational definition, for
   *     sorting TrackStateOnSurface objects
   */
  class TrackStateOnSurfaceComparisonFunction 
    : public std::binary_function<const Trk::TrackStateOnSurface*,
                                  const Trk::TrackStateOnSurface*, bool> {
  public:

    /** Constructor with the fallback direction to use for ordering */
    TrackStateOnSurfaceComparisonFunction( const Amg::Vector3D&     dir)
      :
      m_direction(new  Amg::Vector3D(dir.unit()))
      {}

    /** Copy Ctor */
    TrackStateOnSurfaceComparisonFunction(const TrackStateOnSurfaceComparisonFunction& MCF): 
      m_direction(0)
      {
        if (MCF.m_direction) m_direction = new Amg::Vector3D(*(MCF.m_direction)); 
      }

    /** Destructor */
    virtual ~TrackStateOnSurfaceComparisonFunction()
      {
	delete m_direction;
      }
               
    /** The comparison function defining in what case a TSOS is 'smaller' than
        a second one.
	The idea is to order the TSOS of a track along the track momentum.
	A TSOS is considered to be smaller than another one, if the 
	projection of the momentum of the smaller TSOS on the vector connecting
	the positions of the two TSOS' is positive.
	In case the smaller TSOS does not provide a momentum, the momentum
	of the other TSOS is taken. In case this TSOS does also not provide a
	momentum, a default fallback direction is used. The fallback direction
	has to be specified in the constructer as e.g. the momentum of the
	track's perigee.
    */
    bool operator() (const Trk::TrackStateOnSurface* tsone,
		     const Trk::TrackStateOnSurface* tstwo) const {

      //const Trk::Surface *surf2;

      if (!tsone) return false; 
      if (!tstwo) return true;

      Amg::Vector3D dir = *m_direction;
      Amg::Vector3D pos1(0.,0.,0.);        // dummy init
      Amg::Vector3D pos2(0.,0.,0.);        // dummy init

      if (tsone->trackParameters() && tsone->trackParameters()->momentum().unit().dot(*m_direction)>0.)
	dir = tsone->trackParameters()->momentum().unit();
      else if (tstwo->trackParameters() && tstwo->trackParameters()->momentum().unit().dot(*m_direction)>0.)
	dir = tstwo->trackParameters()->momentum().unit();
     

      if (tsone->trackParameters() )
	pos1 = tsone->trackParameters()->position();
      else if (tsone->measurementOnTrack())
	pos1 = tsone->measurementOnTrack()->globalPosition();

      if (tstwo->trackParameters() )
	pos2 = tstwo->trackParameters()->position();
      else if (tstwo->measurementOnTrack())
	pos2=  tstwo->measurementOnTrack()->globalPosition();
      
      //Trk::GlobalDirection norm=surf2->normal();
      //Amg::Vector3D center=surf2->center();
      
      /* the intersection of the propagation from pos1 in direction dir
	 with the plane surf fulfils  pos1 + t*dir = center + p with p in surf and
	 t to be determined. multiplying with norm using norm perp. to p, yields
	 pos1*norm + t*dir*norm = center*norm. Can be solved for t. 
      */

      double t = (pos2-pos1).dot(dir);

      if (t>0) return true;
      return false;
    }
  private:
    Amg::Vector3D*     m_direction;

  };

} // end of namespace

#endif //MUONTGRECTOOLS_TRACKSTATEONSURFACECOMPARISONFUNCTION_H

