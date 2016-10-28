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
// #include "ZMassConstraint/ConstraintFitOutput.h"
// #include <iostream>

// void ZMassConstraint::ConstraintFitOutput::setFitOutput(const std::vector<TLorentzVector>& particleList, const Amg::MatrixX& covariance)
// {
//   m_particleList = particleList;
//   m_covariance   = covariance;
//   covariancePxPyPzToPhiThetaP();
// }

// void
// ZMassConstraint::ConstraintFitOutput::getCompositeFourVector(TLorentzVector& lv) const
// {
//   lv = TLorentzVector();
//   for( auto lv1 : m_particleList ) lv += lv1;
// }

// void ZMassConstraint::ConstraintFitOutput::covariancePxPyPzToPhiThetaP()
// {
//   Amg::MatrixX Jacobian(5*m_particleList.size(),5*m_particleList.size());
//   Jacobian.setZero();
//   /// build the Jacobian of the phi,theta,P --> px,py,pz
//   for(unsigned int ii=0;ii<m_particleList.size();ii++)
//     {
//       double phi   = m_particleList.at(ii).Phi();
//       double theta = m_particleList.at(ii).Theta();
//       double P     = m_particleList.at(ii).P();
//       Jacobian(    5*ii,     5*ii) = 1.;
//       Jacobian(1 + 5*ii, 1 + 5*ii) = 1.;

//       Jacobian(2 + 5*ii, 2 + 5*ii) = -P * TMath::Sin(theta) * TMath::Sin(phi);
//       Jacobian(2 + 5*ii, 3 + 5*ii) =  P * TMath::Sin(theta) * TMath::Cos(phi);
//       Jacobian(3 + 5*ii, 2 + 5*ii) =  P * TMath::Cos(theta) * TMath::Cos(phi);
//       Jacobian(3 + 5*ii, 3 + 5*ii) =  P * TMath::Cos(theta) * TMath::Sin(phi);
//       Jacobian(3 + 5*ii, 4 + 5*ii) = -P * TMath::Sin(theta);
//       Jacobian(4 + 5*ii, 2 + 5*ii) =      TMath::Sin(theta) * TMath::Cos(phi);
//       Jacobian(4 + 5*ii, 3 + 5*ii) =      TMath::Sin(theta) * TMath::Sin(phi);
//       Jacobian(4 + 5*ii, 4 + 5*ii) =      TMath::Cos(theta);
//     }

//   Amg::MatrixX Jacobianinverse(5*m_particleList.size(),5*m_particleList.size());
  
//   //bool invertible;
//   //Jacobian.computeInverseWithCheck(Jacobianinverse,invertible);
//   //if(!invertible) std::cout << "matrix inversion failed " <<std::endl;
//   Jacobianinverse=Jacobian.inverse();
//   m_covariancePhiThetaP = Jacobianinverse.transpose() * m_covariance * Jacobianinverse;


//   // std::cout << "ZMassConstraint::ConstraintFitOutput::covariancePxPyPzToPhiThetaP\n"
//   //           << m_covariance << "\n" << m_covariancePhiThetaP
//   //           << std::endl;
  
// }


