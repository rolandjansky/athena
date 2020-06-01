/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonCalibSLPropagator.h"
#include "MuonCalibExtraUtils/MuonCalibSimpleGeometry.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>

namespace MuonCalib {
  

  Amg::Vector3D MuonCalibSLPropagator::propagateToPlane( const Amg::Vector3D& pos,      const Amg::Vector3D& dir,     
							       const Amg::Vector3D& planePos, const Amg::Vector3D& planeNormal ) const {
    
    double denom = dir.dot(planeNormal);
    if( std::abs(denom) < 0.00001 ) denom = 0.00001;
    double u = (planeNormal.dot(planePos - pos))/(denom);
    return pos + u * dir;
  }


  Amg::Vector3D MuonCalibSLPropagator::propagateToPlane( const Amg::Vector3D& pos, const Amg::Vector3D& dir, double zPosPlane ) const {

    // calculate intersect pattern measurement plane
    Amg::Vector3D  planePos(0.,0.,zPosPlane);
    Amg::Vector3D planeNormal(0.,0.,1.);
	    
    return propagateToPlane(pos,dir,planePos,planeNormal);
  }

  Amg::Vector3D MuonCalibSLPropagator::propagateToCylinder( const Amg::Vector3D& pos, const Amg::Vector3D& dir, double radiusCylinder ) const {
    double sinThsqinv	= 1./dir.perp2();
    double stepLength	= (-pos.x()*dir.x() - pos.y()*dir.y())* sinThsqinv;
    double deltaRSq	= (radiusCylinder*radiusCylinder - pos.perp2())*sinThsqinv + stepLength*stepLength;
    if (deltaRSq > 0.)  stepLength += std::sqrt(deltaRSq);

    return pos + dir*stepLength;
  }

  Amg::Vector3D MuonCalibSLPropagator::propagateToStraightLine( const Amg::Vector3D& pos, const Amg::Vector3D& dir,
								      const Amg::Vector3D& posLine, const Amg::Vector3D& dirLine ) const {
    double cosAngle	= dirLine.dot(dir);
    double stepLength	= (posLine - pos).dot(dir - dirLine*cosAngle) / (1. - cosAngle*cosAngle);
    return pos + stepLength*dir;

  }
  
  Amg::Vector3D MuonCalibSLPropagator::propagateToPerigee( const Amg::Vector3D& pos, const Amg::Vector3D& dir ) const {
    
    // calculate distance to perigee
    Amg::Vector3D dirLine(0.,0.,1.);
    Amg::Vector3D  posLine(0.,0.,0.);
    return propagateToStraightLine(pos,dir,posLine,dirLine);
  }

  Amg::Vector3D MuonCalibSLPropagator::propagate( const Amg::Vector3D& pos, const Amg::Vector3D& dir, const MuonCalibDisk& disk ) const {
    return propagateToPlane(pos,dir,disk.zPos);
  }
  
  Amg::Vector3D MuonCalibSLPropagator::propagate( const Amg::Vector3D& pos, const Amg::Vector3D& dir, 
							const MuonCalibCylinder& cylinder ) const {
    return propagateToCylinder(pos,dir,cylinder.radius);
  }
  
  Amg::Vector3D MuonCalibSLPropagator::propagate( const Amg::Vector3D& pos, const Amg::Vector3D& dir, const MuonCalibSurface& surf ) const {
    const MuonCalibDisk* disk = dynamic_cast<const MuonCalibDisk*>(&surf);
    if( disk ) return propagate(pos,dir,*disk);

    const MuonCalibCylinder* cylinder = dynamic_cast<const MuonCalibCylinder*>(&surf);
    if( cylinder ) return propagate(pos,dir,*cylinder);
    MsgStream log(Athena::getMessageSvc(),"MuonCalibSLPropagator");
    log<<MSG::WARNING<<"propagation failed, unkown surface type"<<endmsg;
    return Amg::Vector3D(0.,0.,0.);
  }


}
