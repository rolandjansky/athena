/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERINTERFACES_IIMPACTPOINT3DESTIMATOR_H
#define TRKVERTEXFITTERINTERFACES_IIMPACTPOINT3DESTIMATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include <memory> //for unique_ptr

/**
 * @class Trk::ImpactPoint3dEstimator
 *
 * This object calculates the point of minimum distance to the vertex in 3d. Consider that this 
 * point is different from the usually used point of closest approach on the transverse plane.
 *  
 * This algorithm makes use of a simple iterative Newton process 
 * @author N. Giacinto Piacquadio (for the Freiburg Group)
 *
 * -------------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-03-18)
 *
 *   EDM Migration to xAOD - move Trk::Vertex to Amg::Vector3D
 */

namespace Trk
{
  class PlaneSurface;
  class VxTrackAtVertex;

  class IImpactPoint3dEstimator : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID( IImpactPoint3dEstimator, 1, 0 );
    
   /**
    * method calculating the surface (returned), distance and point of closest approach
    */
    virtual std::unique_ptr<PlaneSurface>
    Estimate3dIP(const Trk::TrackParameters* trackPerigee, const Amg::Vector3D* theVertex, double& distance) const = 0;
    virtual std::unique_ptr<PlaneSurface>
    Estimate3dIP(const Trk::NeutralParameters* neutralPerigee, const Amg::Vector3D* theVertex, double& distance) const = 0;

    /**
      * Actual estimate method, changing the state of Trk::VxTrackAtVertex
      */
    virtual bool addIP3dAtaPlane(VxTrackAtVertex &,const Amg::Vector3D & vertex) const = 0;

    /**
      *
      * This method creates the ImpactPoint3dAtaPlane as the parameters of the track at the point of 
      * closest approach in 3d to the given vertex. The parameters and errors are defined on the plane 
      * intersecting the track at point of closest approach, with track ortogonal to the plane and center 
      * of the plane defined as the given vertex.
      */
    virtual const Trk::AtaPlane * IP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const = 0;
    //Same for neutrals
    virtual const Trk::NeutralAtaPlane * IP3dNeutralAtaPlane(const NeutralParameters * initNeutPerigee,const Amg::Vector3D & vertex) const = 0;


 };//end of class definition

}

namespace error
{
  struct ImpactPoint3dEstimatorProblem
  {
    const char* p;
    ImpactPoint3dEstimatorProblem(const char* q) {p=q; }
  };
}

#endif
