/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianLocalToCurvilinear.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


//Trk
#include "TrkEventPrimitives/JacobianLocalToCurvilinear.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
//Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>

Trk::JacobianLocalToCurvilinear::JacobianLocalToCurvilinear( const Trk::CurvilinearUVT& curvUVT,
                                                             const Amg::Vector3D& locX,
                                                             const Amg::Vector3D& locY
                                                             ) :
  AmgMatrix(5,5)()
{
  // initialize to zero
  this->setIdentity();

  (*this)(0,0) = curvUVT.curvU().dot(locX); // d(u)/d(locX)
  (*this)(0,1) = curvUVT.curvU().dot(locY); // d(u)/d(locY)
  (*this)(1,0) = curvUVT.curvV().dot(locX); // d(v)/d(locX)
  (*this)(1,1) = curvUVT.curvV().dot(locY); // d(v)/d(locY)
}

Trk::JacobianLocalToCurvilinear::JacobianLocalToCurvilinear( const Amg::Vector3D& bfield,
															 double qOp,
															 double sinTheta,
                                                             const Trk::CurvilinearUVT& curvUVT,
                                                             const Amg::Vector3D& locX,
                                                             const Amg::Vector3D& locY
                                                             ) :
  AmgMatrix(5,5)()
{
   
   // initialize to zero
   this->setIdentity();
   
   // identical to the straight line case
   (*this)(0,0) = curvUVT.curvU().dot(locX); // d(u)/d(locX)
   (*this)(0,1) = curvUVT.curvU().dot(locY); // d(u)/d(locY)
   (*this)(1,0) = curvUVT.curvV().dot(locX); // d(v)/d(locX)
   (*this)(1,1) = curvUVT.curvV().dot(locY); // d(v)/d(locY)
   // specific for the helix
   const Amg::Vector3D& h = bfield.normalized();
   Amg::Vector3D n(h.cross(curvUVT.curvT()));   // direction normal to track and magnetic field direction
   double alpha = n.mag();                    // | h x t | projection of track normal to magnetic field direction
   n /= alpha;                                // normalization
   double B = bfield.mag();
    
   // -> Psi and pathlength related variables
   double Q   =  - B * Gaudi::Units::c_light * qOp; 
   
   // prepare the parameters
   double tlx = curvUVT.curvT().dot(locX);   // t * locX
   double tly = curvUVT.curvT().dot(locY);   // t * locY
   double nu  = n.dot(curvUVT.curvU());      // n * u
   double nv  = n.dot(curvUVT.curvV());      // n * v

   double oneOverSinTheta = 1./sinTheta; // (helix.startParameters().sinTheta());

   // fill the components
   (*this)(2,0) =  -alpha*Q*oneOverSinTheta*nu*tlx; // d(phi)/d(locX)
   (*this)(2,1) = - alpha*Q*oneOverSinTheta*nu*tly; // d(phi)/d(locY)
    
   (*this)(3,0) =   alpha*Q*nv*tlx;               // d(theta)/d(locX)
   (*this)(3,1) =   alpha*Q*nv*tly;               // d(theta)/d(locY)

}


/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::JacobianLocalToCurvilinear& jac)
{ 
	sl << std::setiosflags(std::ios::fixed);
        sl << std::setprecision(6);
        sl << MSG::DEBUG << "Trk::JacobianLocalToCurvilinear"                          << std::endl;
        sl << "______________________________________________________________________" << std::endl;
	for (int irow = 0; irow<5; irow++){
            for (int icol =0; icol<5; icol++){
                sl <<  (jac)(irow,icol);
                if (irow < 4 || icol < 4 ) { sl << "     ";}
            }
            sl << std::endl;        
        }
        sl << "______________________________________________________________________";      
        return sl;
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::JacobianLocalToCurvilinear& jac)
{  
	sl << std::setiosflags(std::ios::fixed);
        sl << std::setprecision(7);        
        sl << "Trk::JacobianLocalToCurvilinear " << std::endl;
        sl << "______________________________________________________________________" << std::endl;
	for (int irow = 0; irow<5; irow++){
            for (int icol =0; icol<5; icol++){
                sl <<  (jac)(irow,icol);
                if (irow < 4 || icol < 4 ) { sl << "     "; }
            }
            sl << std::endl;        
        }
       sl << "______________________________________________________________________";	
       return sl;   
}

