/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/PlanarFlow.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TLorentzVector.h"

using namespace std;
using namespace JetSubStructureUtils;

double PlanarFlow::result(const fastjet::PseudoJet &jet) const
{

  double PF=-1.;
  if(jet.m() == 0.0 ) return PF;
  if(jet.constituents().size() == 0 ) return PF;
 
  vector<fastjet::PseudoJet> constit_pseudojets = jet.constituents();

  TMatrixDSym MomentumTensor(2);
  //Planar flow
  double phi0=jet.phi();
  double eta0=jet.eta();

  double nvec[3];
  nvec[0]=(cos(phi0)/cosh(eta0));
  nvec[1]=(sin(phi0)/cosh(eta0));
  nvec[2]=tanh(eta0);

  //this is the rotation matrix
  double RotationMatrix[3][3];

  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      RotationMatrix[i][j] = 0.;
    }
  }

  const double mag3 = sqrt(nvec[0]*nvec[0] + nvec[1]*nvec[1]+ nvec[2]*nvec[2]);
  const double mag2 = sqrt(nvec[0]*nvec[0] + nvec[1]*nvec[1]);

  if(mag3 <= 0) {
    // Rotation axis is null
    return PF;
  }

  const double inv_mag3 = 1. / mag3;

  double ctheta0 = nvec[2]*inv_mag3;
  double stheta0 = mag2*inv_mag3;
  double cphi0 = (mag2>0.) ? nvec[0]/mag2:0.;
  double sphi0 = (mag2>0.) ? nvec[1]/mag2:0.;

  RotationMatrix[0][0] =- ctheta0*cphi0;
  RotationMatrix[0][1] =- ctheta0*sphi0;
  RotationMatrix[0][2] = stheta0;
  RotationMatrix[1][0] = sphi0;
  RotationMatrix[1][1] =- 1.*cphi0;
  RotationMatrix[1][2] = 0.;
  RotationMatrix[2][0] = stheta0*cphi0;
  RotationMatrix[2][1] = stheta0*sphi0;
  RotationMatrix[2][2] = ctheta0;


  for(vector<fastjet::PseudoJet>::iterator cit=constit_pseudojets.begin(); cit != constit_pseudojets.end();
      ++cit) {
    const fastjet::PseudoJet & cp = *cit;
    TLorentzVector p = TLorentzVector(cp.px(),cp.py(),cp.pz(),cp.e());
    double n=1./(cp.e()*jet.m());
    double px_rot = RotationMatrix[0][0] * (p.Px())+RotationMatrix[0][1]
      * (p.Py())+RotationMatrix[0][2]*(p.Pz());
    double py_rot = RotationMatrix[1][0] * (p.Px())+RotationMatrix[1][1]
      * (p.Py())+RotationMatrix[1][2]*(p.Pz());
    double pz_rot = RotationMatrix[2][0] * (p.Px())+RotationMatrix[2][1]
      * (p.Py())+RotationMatrix[2][2]*(p.Pz());

    TLorentzVector prot;
    prot.SetPxPyPzE(px_rot, py_rot, pz_rot, p.E() );

    MomentumTensor(0,0) += n * prot.Px() * prot.Px();
    MomentumTensor(0,1) += n * prot.Py() * prot.Px();
    MomentumTensor(1,0) += n * prot.Px() * prot.Py();
    MomentumTensor(1,1) += n * prot.Py() * prot.Py();
  }

  TMatrixDSymEigen eigen(MomentumTensor);
  TVectorD Lambda = eigen.GetEigenValues();
  double num = 4*Lambda[0]*Lambda[1];
  double den = (Lambda[0]+Lambda[1]) * (Lambda[0]+Lambda[1]);
  if ( fabs(den) < 1.e-20 ) return PF;
  PF = num/den;
  return PF;
}
