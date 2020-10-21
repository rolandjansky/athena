/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaConditions/EMClusterEtaErrorsMatrix.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <cmath>
#include <utility>


// Constructors
EMClusterEtaErrorsMatrix::EMClusterEtaErrorsMatrix() : 
  EMClusterErrorsMatrix()
{
}


EMClusterEtaErrorsMatrix::EMClusterEtaErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
						   const std::string& textDescription) : 
  EMClusterErrorsMatrix(axes, textDescription)
{
}

/// Add the outcome of the test at the given "eta" and "pt" value, i.e. if the reconstruction was successful or not
StatusCode EMClusterEtaErrorsMatrix::setError(double eta, float a) // a should be in rad, not mrad
{
  std::vector<double> v; // will actually be converted to a Float_t when stored in db.
  v.push_back(eta);

  EMClusterErrorsEntry apEntry;
  apEntry.push_back(a);
  
  return EMClusterErrorsMatrix::setError(v, apEntry);
}

/// Get the efficiency for the given "eta" and "pt" value
// which eta should we use? Should look at note.
double EMClusterEtaErrorsMatrix::getError(double eta, double clusterEnergy) const
{
  
  const EMClusterErrorsEntry *params = EMClusterErrorsMatrix::getError(eta);

  if (!params) return -1.0;

  // for protection, to not divide by 0;
  if (clusterEnergy < 1.0) clusterEnergy = 1.0;
  if (fabs(eta) > 8.0) eta = 8.0;
  
  const double clusterEnergyGeV = clusterEnergy*(1./Gaudi::Units::GeV);  //GeV
  
  /** Error on theta = C(eta) mrad/sqrt(Energy) */
  /** NOTE: Eta dependence of C  will be implemented later. C=70mrad based on EG6 note*/ 
  
  const double a = params->at(0); // a should be in radians, not mrad
  const double sigma_theta = a /sqrt(clusterEnergyGeV);
  
  /** Extract theta from eta = -ln(tan(theta/2)) */
  const double theta = 2.*atan(exp(-eta));

//   std::cout << "EMClusterEtaErrorEntry called with (eta, energy) = (" << eta << ", " << clusterEnergy << ")\n"; 
//   std::cout << "  returned parameters a = " << a << "\n"; 
//   std::cout << "  returned error = " << fabs(sigma_theta/sin(theta)) << std::endl; 
  
  /** sigma_eta */ 
  return ( fabs(sigma_theta/sin(theta)) ); 
}
