/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   JacobianCurvilinearToLocal.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


//Trk
#include "TrkEventPrimitives/JacobianCurvilinearToLocal.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
//Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>


/** Calculates the Jacobian
  * for straight line track model
  */
Trk::JacobianCurvilinearToLocal::JacobianCurvilinearToLocal(const CurvilinearUVT& curvUVT,
                                                            const Amg::Vector3D& locX,
                                                            const Amg::Vector3D& locY,
                                                            const Amg::Vector3D& locZ
                                                            ) :
  AmgMatrix(5,5)()
{
    //initialize to zero
    this->setIdentity();

    double locZdotCurvT = locZ.dot(curvUVT.curvT());
    (*this)(0,0) =   curvUVT.curvV().dot(locY)/locZdotCurvT;    // d(locX)/d(u)
    (*this)(0,1) =   - curvUVT.curvU().dot(locY)/locZdotCurvT;  // d(locX)/d(v)
    (*this)(1,0) =   - curvUVT.curvV().dot(locX)/locZdotCurvT;  // d(locY)/d(u)
    (*this)(1,1) =   curvUVT.curvU().dot(locX)/locZdotCurvT;    // d(locY)/d(v)

}

/** Calculates the Jacobian
  * for helical track model
  */
Trk::JacobianCurvilinearToLocal::JacobianCurvilinearToLocal(
  const Amg::Vector3D& bfield,
  double qOp,
  double sinTheta,
  const CurvilinearUVT& curvUVT,
  const Amg::Vector3D& locX,
  const Amg::Vector3D& locY,
  const Amg::Vector3D& locZ)
  : AmgMatrix(5, 5)()
{
    this->setIdentity();
    
    // prepare the dot products
    double oneOverZt = 1./locZ.dot(curvUVT.curvT());   // 1./locZ * t
    double ux           = curvUVT.curvU().dot(locX);   // u * locX
    double uy           = curvUVT.curvU().dot(locY);   // u * locY
    double uz           = curvUVT.curvU().dot(locZ);   // u * locZ
    double vx           = curvUVT.curvV().dot(locX);   // v * locX
    double vy           = curvUVT.curvV().dot(locY);   // v * locY
    double vz           = curvUVT.curvV().dot(locZ);   // v * locZ
    // specific for the helix
    const Amg::Vector3D& h = bfield.normalized();
    Amg::Vector3D n(h.cross(curvUVT.curvT()));   // direction normal to track and magnetic field direction
    double alpha = n.mag();                    // | h x t | projection of track normal to magnetic field direction
    n /= alpha;                                // normalization
    double B = bfield.mag();
    
    // -> Psi and pathlength related variables
    double Q   =  - B * Gaudi::Units::c_light * qOp; 
    double alphaQ = alpha*Q;    

    double un = curvUVT.curvU().dot(n);
    double vn = curvUVT.curvV().dot(n);
    // fill the components
    (*this)(0,0) =     vy*oneOverZt;                   // d(locX)/d(u)
    (*this)(0,1) =   - uy*oneOverZt;                   // d(locX)/d(v)
    (*this)(1,0) =   - vx*oneOverZt;                   // d(locY)/d(u)
    (*this)(1,1) =     ux*oneOverZt;                   // d(locY)/d(v)
    (*this)(2,0) =   - alphaQ/sinTheta*un*uz*oneOverZt;  // d(phi)/d(u)
    (*this)(2,1) =   - alphaQ/sinTheta*un*vz*oneOverZt;  // d(phi)/d(v)
    (*this)(3,0) =     alphaQ*vn*uz*oneOverZt;          // d(theta)/d(v)
    (*this)(3,1) =     alphaQ*vn*vz*oneOverZt;          // d(theta)/d(v)  

}



/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::JacobianCurvilinearToLocal& jac)
{ 
	sl << std::setiosflags(std::ios::fixed);
        sl << std::setprecision(6);
        sl << MSG::DEBUG << "Trk::JacobianCurvilinearToLocal"                          << std::endl;
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

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::JacobianCurvilinearToLocal& jac)
{  
	sl << std::setiosflags(std::ios::fixed);
        sl << std::setprecision(7);        
        sl << "Trk::JacobianCurvilinearToLocal " << std::endl;
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
