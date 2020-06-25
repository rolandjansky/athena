/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERUTILS_IMPACTPOINT3DESTIMATOR_H
#define TRKVERTEXFITTERUTILS_IMPACTPOINT3DESTIMATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

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

namespace Trk
{
  class IExtrapolator;

  class ImpactPoint3dEstimator : public extends<AthAlgTool, IImpactPoint3dEstimator>
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
  /**
   * Default constructor due to Athena interface
   */
    ImpactPoint3dEstimator(const std::string& t, const std::string& n, const IInterface*  p);

    virtual ~ImpactPoint3dEstimator();
    
    virtual std::unique_ptr<PlaneSurface>
    Estimate3dIP(const Trk::TrackParameters* trackPerigee,
                 const Amg::Vector3D* theVertex,
                 double& distance) const override;

    virtual std::unique_ptr<PlaneSurface>
    Estimate3dIP(const Trk::NeutralParameters* neutralPerigee,
                 const Amg::Vector3D* theVertex,
                 double& distance) const override;
   
    /**
      * Actual estimate method, changing the state of Trk::VxTrackAtVertex
      */
    virtual bool addIP3dAtaPlane(VxTrackAtVertex &,const Amg::Vector3D & vertex) const override;

    /**
      *
      * This method creates the ImpactPoint3dAtaPlane as the parameters of the track at the point of 
      * closest approach in 3d to the given vertex. The parameters and errors are defined on the plane 
      * intersecting the track at point of closest approach, with track ortogonal to the plane and center 
      * of the plane defined as the given vertex.
      */
    virtual const Trk::AtaPlane * IP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const override;

    //Same for neutrals
    virtual const Trk::NeutralAtaPlane * IP3dNeutralAtaPlane(const NeutralParameters * initNeutPerigee,const Amg::Vector3D & vertex) const override;

  private:
    template<typename T>
    std::unique_ptr<PlaneSurface>
    Estimate3dIPNoCurvature(const T*,
                            const Amg::Vector3D* theVertex,
                            double& distance) const;

    
    
    ToolHandle< Trk::IExtrapolator > m_extrapolator;
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey 
          {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

    int m_maxiterations;
    double m_precision;
    
  };

}

#endif
