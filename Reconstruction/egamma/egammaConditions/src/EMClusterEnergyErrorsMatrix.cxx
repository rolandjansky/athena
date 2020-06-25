/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaConditions/EMClusterEnergyErrorsMatrix.h"
#include <cmath>
#include <utility>

// Constructors
EMClusterEnergyErrorsMatrix::EMClusterEnergyErrorsMatrix() : 
  EMClusterErrorsMatrix()
{
}


EMClusterEnergyErrorsMatrix::EMClusterEnergyErrorsMatrix(const std::vector<EMAPMatrixAxis> &axes, 
							 const std::string& textDescription) : 
  EMClusterErrorsMatrix(axes, textDescription)
{
}

StatusCode EMClusterEnergyErrorsMatrix::setError(double eta, float a, float b, float c)
{
  std::vector<double> v; // will actually be converted to a Float_t when stored in db.
  v.push_back(eta);
  
  EMClusterErrorsEntry apEntry;
  apEntry.push_back(a);
  apEntry.push_back(b);
  apEntry.push_back(c);
  
  return EMClusterErrorsMatrix::setError(v, apEntry);
}

/// Get the efficiency for the given "eta" and "pt" value
double EMClusterEnergyErrorsMatrix::getError(double eta, double clusterEnergy) const
{

  // for protection
  if (clusterEnergy < 1.0) clusterEnergy = 1.0;
  if (fabs(eta) > 8.0) eta = 8.0;
    
  const EMClusterErrorsEntry *params = EMClusterErrorsMatrix::getError(eta);

  if (!params) return -1.0;
  
  double clusterEnergyGeV = clusterEnergy*1e-3;  //GeV
  
  /** sigma_E (eta,E) = a + b*sqrt(E) + c*E */
  /** NOTE: The a, b and c parameters will be tuned later*/
  const double a = params->at(0);
  const double b = params->at(1);
  const double c = params->at(2);
  
  double function = a*a + (b*b*clusterEnergyGeV) 
    + ((c*clusterEnergyGeV)*(c*clusterEnergyGeV));
  
//   std::cout << "EMClusterEnergyErrorEntry called with (eta, energy) = (" << eta << ", " << clusterEnergy << ")\n"; 
//   std::cout << "  returned parameters (a, b, c) = (" << a << ", " << b << ", " << c  << ")\n"; 
//   std::cout << "  returned error = " << sqrt(function)*1000.0 << std::endl; 

  return  sqrt(function)*1000.0;  //MeV   
   
}

