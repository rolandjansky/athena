/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianLocalAnglesPhiTheta.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianLocalAnglesPhiTheta.h"
#include <cmath>

Trk::JacobianLocalAnglesPhiTheta::JacobianLocalAnglesPhiTheta(const double angleXZ, const double angleYZ, const Amg::RotationMatrix3D& rot): 
    AmgMatrix(2,2)()
{

    this->setIdentity();

// Rot is a 3x3 rotation matrix from local 3-vector to global 3-vector
 
    double cx = std::cos(angleXZ);
    double sx = std::sin(angleXZ);
    double cy = std::cos(angleYZ);
    double sy = std::sin(angleYZ); 
    double norm = std::sqrt(cx*sy*cx*sy + cy*sx*cy*sx + sx*sy*sx*sy);
    double zinv = norm/(sx*sy) ;  
//
//  local direction x = cx * sy / norm
//                  y = cy * sx / norm
//                  z = sx * sy / norm 
//
    
// Determine Global theta and phi 

    Amg::Vector3D locdir(cx*sy/norm,cy*sx/norm,sx*sy/norm);
    Amg::Vector3D gdir = rot*locdir;
    double phi = gdir.phi();
    double theta = gdir.theta();
//    std::cout << " phi " << phi << " theta " << theta << std::endl; 
    double cp = std::cos(phi);
    double sp = std::sin(phi);
    double ct = std::cos(theta);
    double st = std::sin(theta);
    AmgMatrix(3,2) J2 = AmgMatrix(3,2)::Zero(); // row, column
//
//  derivatives dx/dphi  dx/dtheta  
//              dy/dphi  dy/dtheta
//              dz/dphi  dz/dtheta
//
//              x = std::sin(theta)*std::cos(phi) 
//              y = std::sin(theta)*std::sin(phi) 
//              z = std::cos(theta) 
//

    J2(0,0) = -st*sp;
    J2(1,0) = st*cp;
    J2(0,1) = ct*cp;
    J2(1,1) = ct*sp;
    J2(2,1) = -st;
 
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

    const AmgMatrix(3,3)& Rot  = rot;
    AmgMatrix(2,2) Jinv = AmgMatrix(2,2)::Zero();
//
//  Jacobian Inverse: lefthand local righthand global
//
    AmgMatrix(3,3) RI = Rot.inverse(); 
    Jinv = J1*(RI*J2); 
//
//  Default Matrix
//
    (*this)(0,0) = 1.;
    (*this)(0,1) = 0.;
    (*this)(1,0) = 0.;
    (*this)(1,1) = 1.;
//
// Store Jacobian  J:  right hand local
//
//    if (succes==0) {
//      AmgMatrix(2,2) J = Jinv.inverse(succes);
//      if (succes==0) {
////        std::cout << " JacobianLocalAnglesPhiTheta All there " << std::endl;  
//        (*this)(0,0) = J(0,0);
//        (*this)(0,1) = J(0,1);
//        (*this)(1,0) = J(1,0);
//        (*this)(1,1) = J(1,1);
//      }
//    } 
}

