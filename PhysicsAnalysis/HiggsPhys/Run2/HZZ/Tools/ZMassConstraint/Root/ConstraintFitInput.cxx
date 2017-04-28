/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// A C++ implementation of Mass constraint fitting
// 23/09/2006
// K. Nikolopoulos
// --- * --- * --- * --- * ---* --- * --- * --- * ---
//
//
//
//
// #include "ZMassConstraint/ConstraintFitInput.h"

// ZMassConstraint::ConstraintFitInput::ConstraintFitInput():
//     m_isOK(true)
// {
// }

// void ZMassConstraint::ConstraintFitInput::addConstituent_FourVector_d0z0PhiThetaP(const TLorentzVector& vector,
//                                                                                   const AmgMatrix(5,5)& covar,
//                                                                                   bool isOK)
// {
//     m_particles.push_back(vector);
//     m_covariances.push_back(covar);
//     if (m_isOK) m_isOK = isOK; // do not reset isOK if it has already been set to false!

//     //going from d0,z0,phi,theta,P --> d0,z0,px,py,pz
//     double phi   = vector.Phi();
//     double theta = vector.Theta();
//     double P     = vector.P();
//     //std::cout << ":::::L \t"<<P<<"\t"<< phi<<"\t"<<theta<<std::endl;
//     AmgMatrix(5,5) jacobian;
//     jacobian.setZero();
//     jacobian(0,0) = 1.;
//     jacobian(1,1) = 1.;
//     jacobian(2,2) = -P * TMath::Sin(theta) * TMath::Sin(phi);
//     jacobian(2,3) =  P * TMath::Sin(theta) * TMath::Cos(phi);
//     jacobian(3,2) =  P * TMath::Cos(theta) * TMath::Cos(phi);
//     jacobian(3,3) =  P * TMath::Cos(theta) * TMath::Sin(phi);
//     jacobian(3,4) = -P * TMath::Sin(theta);
//     jacobian(4,2) =      TMath::Sin(theta) * TMath::Cos(phi);
//     jacobian(4,3) =      TMath::Sin(theta) * TMath::Sin(phi);
//     jacobian(4,4) =      TMath::Cos(theta);
	
//     AmgMatrix(5,5) newcovariance = jacobian.transpose() * covar * jacobian;
//     m_cartCovariances.push_back(newcovariance);

//     // std::cout << "initial\n"<< covar << "final \n" << newcovariance << "jac\n"
//     //         << jacobian << std::endl;

// }




