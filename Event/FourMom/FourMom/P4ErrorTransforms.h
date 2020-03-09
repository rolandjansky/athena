/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef P4ErrorTransforms_H
#define P4ErrorTransforms_H
#include <memory>
class ErrorMatrixEEtaPhiM;
class ErrorMatrixPxPyPzE;

namespace P4ErrorTransforms {

std::unique_ptr<ErrorMatrixEEtaPhiM> toEEtaPhiM( const ErrorMatrixPxPyPzE& em,
                                                 double px, double py, double pz, double E);
std::unique_ptr<ErrorMatrixPxPyPzE> toPxPyPzE( const ErrorMatrixEEtaPhiM& em,
                                               double E, double eta, double phi, double M);

}


#endif
