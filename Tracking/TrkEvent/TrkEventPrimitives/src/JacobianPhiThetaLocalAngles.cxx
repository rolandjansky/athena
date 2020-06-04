/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPhiThetaLocalAngles.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianPhiThetaLocalAngles.h"
#include <cmath>


Trk::JacobianPhiThetaLocalAngles::JacobianPhiThetaLocalAngles(const double phi, const double theta, const Amg::RotationMatrix3D& rot) :
    AmgMatrix(2,2)()
{
   // initialize
   this->setIdentity();
   
   // Rot is a 3x3 rotation matrix from global 3-vector to local 3-vector
    double cp = std::cos(phi);
    double sp = std::sin(phi);
    double ct = std::cos(theta);
    double st = std::sin(theta);
 
    AmgMatrix(3,2) J2 = AmgMatrix(3,2)::Zero();
//
//  derivatives dx/dphi  dx/dtheta  
//              dy/dphi  dy/dtheta
//              dz/dphi  dz/dtheta
//
//              x = sin(theta)*cos(phi) 
//              y = sin(theta)*sin(phi) 
//              z = cos(theta) 
//
    J2(0,0) = -st*sp;
    J2(1,0) = st*cp;
    J2(0,1) = ct*cp;
    J2(1,1) = ct*sp;
    J2(2,1) = -st;

// Determine Local theta and phi 

    Amg::Vector3D gdir(st*cp,st*sp,ct);
    Amg::Vector3D ldir = rot*gdir;
    double angleXZ = std::atan2(ldir.z(),ldir.x());
    double angleYZ = std::atan2(ldir.z(),ldir.y());

    double cx = std::cos(angleXZ);
    double sx = std::sin(angleXZ);
    double cy = std::cos(angleYZ);
    double sy = std::sin(angleYZ); 
    double norm = std::sqrt(cx*sy*cx*sy + cy*sx*cy*sx + sx*sy*sx*sy);
    double zinv = norm/(sx*sy) ;  

//    std::cout << " Local angles XZ " <<  angleXZ << " YZ " << angleYZ << std::endl;
//
//  local direction x = cx * sy / norm
//                  y = cy * sx / norm
//                  z = sx * sy / norm 
//
    AmgMatrix(2,3) J1 = AmgMatrix(2,3)::Zero();
//
//  derivatives d alphaXZ/dx  d alphaXZ/dy  d alphaXZ/dz   
//              d alphaYZ/dx  d alphaYZ/dy  d alphaYZ/dz  
//
//        tan(alphaXZ) = z / x
//        tan(alhpaYZ) = z / y
//  
    J1(0,0) = -sx*sx*zinv;
    J1(1,1) = -sy*sy*zinv;
    J1(0,2) = cx*sx*zinv;
    J1(1,2) = cy*sy*zinv;

    const AmgMatrix(3,3)& Rot = rot;
    AmgMatrix(2,2) J = AmgMatrix(2,2)::Zero();
//
//  Jacobian: lefthand local righthand global
//
    J = J1*(Rot*J2);
//
// Store Jacobian  J
//
    (*this)(0,0) = J(0,0);
    (*this)(0,1) = J(0,1);
    (*this)(1,0) = J(1,0);
    (*this)(1,1) = J(1,1);
}

