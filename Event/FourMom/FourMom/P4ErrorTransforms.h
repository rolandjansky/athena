/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef P4ErrorTransforms_H
#define P4ErrorTransforms_H

class ErrorMatrixEEtaPhiM;
class ErrorMatrixPxPyPzE;

namespace P4ErrorTransforms {

  ErrorMatrixEEtaPhiM* toEEtaPhiM( const ErrorMatrixPxPyPzE& em,
				   double px, double py, double pz, double E);

  ErrorMatrixPxPyPzE* toPxPyPzE( const ErrorMatrixEEtaPhiM& em,
				 double E, double eta, double phi, double M);

}


#endif
