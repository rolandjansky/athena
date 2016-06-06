/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERUTILS_IMPACTPOINT3DESTIMATOR_H
#define TRKVERTEXFITTERUTILS_IMPACTPOINT3DESTIMATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "TrkNeutralParameters/NeutralParameters.h"

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
 * EDM Migration to xAOD - move Trk::Vertex to Amg::Vector3D
 *
 */

namespace MagField {
  class IMagFieldSvc;
}

namespace Trk
{
  class IExtrapolator;

  class ImpactPoint3dEstimator : public AthAlgTool, virtual public IImpactPoint3dEstimator
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
    
  /**
   * Default constructor due to Athena interface
   */
    ImpactPoint3dEstimator(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    * Destructor
    */
    ~ImpactPoint3dEstimator();
    
   /**
    * Actual estimate method
    */
//    void Estimate3dIP(const ParametersBase* trackPerigee,const Vertex* theVertex);
    
   /**
    * Access to the plane through 3D impact point
    */  
 //   PlaneSurface* getPlaneThrough3dIP() const;
   
   
   /**
    * New method implementing the features of two above methods at once
    */
    template<typename T> PlaneSurface* _Estimate3dIPNoCurvature(const T*, const Amg::Vector3D* theVertex) const;

    PlaneSurface* Estimate3dIP(const Trk::TrackParameters* trackPerigee, const Amg::Vector3D* theVertex) const;

    PlaneSurface* Estimate3dIP(const Trk::NeutralParameters* neutralPerigee, const Amg::Vector3D* theVertex) const;
   
    /**
     * Access to the 3D impact point
     */
    Amg::Vector3D* get3dIP() const;

    /**
     * Access to the IP 3D distance
     */
    double getDistance() const;
 
    /**
      * Actual estimate method, changing the state of Trk::VxTrackAtVertex
      */
    bool addIP3dAtaPlane(VxTrackAtVertex &,const Amg::Vector3D & vertex) const;

    /**
      *
      * This method creates the ImpactPoint3dAtaPlane as the parameters of the track at the point of 
      * closest approach in 3d to the given vertex. The parameters and errors are defined on the plane 
      * intersecting the track at point of closest approach, with track ortogonal to the plane and center 
      * of the plane defined as the given vertex.
      */
    Trk::AtaPlane * IP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const;

    //Same for neutrals
    Trk::NeutralAtaPlane * IP3dNeutralAtaPlane(const NeutralParameters * initNeutPerigee,const Amg::Vector3D & vertex) const;

  private:
    
    
    ToolHandle< Trk::IExtrapolator > m_extrapolator;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;
    
    int m_maxiterations;
    double m_precision;
    
    mutable Amg::Vector3D* m_vertex;
    
    mutable double m_distance;
    

  };


  inline double ImpactPoint3dEstimator::getDistance() const
  {
    return m_distance;
  }
}

#endif
