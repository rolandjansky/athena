/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaConditions/EMClusterEtaPosErrorsMatrix.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>
#include <utility>

using CLHEP::GeV;

// Constructors
EMClusterEtaPosErrorsMatrix::EMClusterEtaPosErrorsMatrix() : 
  EMClusterErrorsMatrix()
{
}


EMClusterEtaPosErrorsMatrix::EMClusterEtaPosErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
						   const std::string& textDescription) : 
  EMClusterErrorsMatrix(axes, textDescription)
{
}

/// Add the outcome of the test at the given "eta" and "pt" value, i.e. if the reconstruction was successful or not
StatusCode EMClusterEtaPosErrorsMatrix::setError(double eta, float a, float b) // a should be in rad, not mrad
{

  std::vector<double> v; // will actually be converted to a Float_t when stored in db.
  v.push_back(eta);

  EMClusterErrorsEntry apEntry;
  apEntry.push_back(a);
  apEntry.push_back(b);
  
  return EMClusterErrorsMatrix::setError(v, apEntry);
}

/// Get the efficiency for the given "cluster energy" 
double EMClusterEtaPosErrorsMatrix::getError(double eta, double clusterEnergy) const
{
  
  const EMClusterErrorsEntry *params = EMClusterErrorsMatrix::getError(eta);

  if (!params) return -1.0;

  // for protection, to not divide by 0;
  if (clusterEnergy < 1.0) clusterEnergy = 1.0;
  
  const double clusterEnergyGeV = clusterEnergy*(1./GeV);  //GeV
  
  const double a = params->at(0); // "a" from DB
  const double b = params->at(1); // "b" from DB
  /** sigma_etaS1 */ 
  return ( a*sqrt(b/clusterEnergyGeV));
}
