/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          Chi2TrackCompatibilityEstimator.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitterUtils/Chi2TrackCompatibilityEstimator.h"

#include "VxVertex/VxTrackAtVertex.h"

#include "TrkSurfaces/PlaneSurface.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParametersBase/ParametersBase.h"

#include <cmath>

namespace Trk
{

  StatusCode Chi2TrackCompatibilityEstimator::initialize() 
  { 
    ATH_MSG_DEBUG( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }

  StatusCode Chi2TrackCompatibilityEstimator::finalize() 
  {
    ATH_MSG_DEBUG( "Finalize successful"  );
    return StatusCode::SUCCESS;
  }

  
  void Chi2TrackCompatibilityEstimator::estimate(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const
  {
    vtxTrack.setVtxCompatibility(compatibility(vtxTrack,vertex));
  }
 
  
  template <class T> float Chi2TrackCompatibilityEstimator::_compatibility(T & myAtaPlane,const Amg::Vector3D & vertex) const {

    ATH_MSG_VERBOSE ("Given plane: " << myAtaPlane->associatedSurface() );

    //now, once you have the AtaPlane, project the actual vertex on the plane.
    Amg::RotationMatrix3D myPlaneRotation=myAtaPlane->associatedSurface().transform().rotation();
    Amg::Vector3D myTranslation=myAtaPlane->associatedSurface().transform().translation();

    //get the x and y direction of the plane
    Amg::Vector3D myXdirPlane=myPlaneRotation.col(0);
    Amg::Vector3D myYdirPlane=myPlaneRotation.col(1);
    //CLHEP::Hep3Vector myZdirPlane=myRotation.colZ();

    #ifdef Chi2TrackCompatibilityEstimator_DEBUG
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "Position is: " << vertex << std::endl;
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "Rotation is: " << myPlaneRotation << std::endl;
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "Norm of colX() is: " << myXdirPlane.mag() << " of colY() is: " << myYdirPlane.mag() << std::endl;
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "Translation is: " << myTranslation << std::endl;
    #endif

    Amg::Vector3D vertexMinusCenter=vertex-myTranslation;

    #ifdef Chi2TrackCompatibilityEstimator_DEBUG
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "VertexMinusCenter: " << vertexMinusCenter << std::endl;
    #endif

    Amg::Vector2D myVertexlocXlocY;
    myVertexlocXlocY[0]=vertexMinusCenter.dot(myXdirPlane);
    myVertexlocXlocY[1]=vertexMinusCenter.dot(myYdirPlane);

    #ifdef Chi2TrackCompatibilityEstimator_DEBUG
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "Vertex local coordinates: " << myVertexlocXlocY << std::endl;
    #endif

    /* ******Maybe this can be still helpfull
    //create a default xy plane
    Plane3D trackPlane();
    
    //rotate and traslate it according to the plane associated to MeasuredAtaPlane
    trackPlane.transform(myAtaPlane->associatedSurface()->transform());
    
    //now project the vertex onto the plane
    Point3D theVertexOnPlane=trackPlane.point(Point3D(vertex.x(),vertex.y(),vertex.z()));
    
    //now express the point found in local coordinates
    */

    const AmgSymMatrix(5) * covar = (myAtaPlane->covariance());
    AmgSymMatrix(2) myWeightXY = (*covar).block<2,2>(0,0);
    myWeightXY = myWeightXY.inverse().eval();

    Amg::Vector2D myXY;
    myXY[0]=myAtaPlane->parameters()[Trk::locX]-myVertexlocXlocY[0];
    myXY[1]=myAtaPlane->parameters()[Trk::locY]-myVertexlocXlocY[1];

    #ifdef Chi2TrackCompatibilityEstimator_DEBUG
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "Parameters local coordinates: " << " x: " <<myAtaPlane->parameters()[Trk::locX] <<
      " y: " << myAtaPlane->parameters()[Trk::locY] << std::endl;
    std::cout << std::setprecision(10) << "Chi2TrackCompatibilityEstimator: " << "VertexToTrack coordinates: " << myXY << std::endl;
    #endif

    ATH_MSG_DEBUG ("The weight assigned to the track is " << myXY.dot(myWeightXY*myXY) );
    ATH_MSG_VERBOSE ("coming from X: " << myXY[0] << " and error: "
        << std::sqrt((*(myAtaPlane->covariance()))(Trk::locX,Trk::locX)));
    ATH_MSG_VERBOSE ("and from z0: " << myXY[1] << " and error: "
        << std::sqrt((*(myAtaPlane->covariance()))(Trk::locY,Trk::locY)));

    return myXY.dot(myWeightXY*myXY);
  }


  float Chi2TrackCompatibilityEstimator::compatibility(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const
  {
    //Looking for a AtaPlane object (track)
    const Trk::AtaPlane * myAtaPlane=vtxTrack.ImpactPoint3dAtaPlane();
    if (myAtaPlane!=nullptr && myAtaPlane->covariance()!=nullptr) {
      return _compatibility(myAtaPlane, vertex);
    }

    //looking for a NeutralAtaPlane object (neutral)
    const Trk::NeutralAtaPlane * myNeutralAtaPlane=vtxTrack.ImpactPoint3dNeutralAtaPlane();
    if (myNeutralAtaPlane!=nullptr && myNeutralAtaPlane->covariance()!=nullptr){
      return _compatibility(myNeutralAtaPlane, vertex);
    }
    
    ATH_MSG_WARNING( " No compatibility plane attached to the VxTrackAtVertex. Compatibility couldn't be found... 0 compatibility returned."  );
    return 100;
  }
}
