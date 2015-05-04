/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloMonTool.h"

#include <cmath> 

#include <sstream> 


double DeltaPhi(double phi_1, double phi_2) 
{
  phi_1 = (phi_1 > M_PI) ? phi_1 - (2 * M_PI) : phi_1; 
  phi_2 = (phi_2 > M_PI) ? phi_2 - (2 * M_PI) : phi_2; 
  
  double dphi = fabs(phi_1 - phi_2);
  
  if(dphi > M_PI) dphi = (2 * M_PI) - dphi;
  
  return dphi;
}


double DeltaR(double eta_1, double phi_1, double eta_2, double phi_2) 
{
  double dR = sqrt( ((eta_1 - eta_2)*(eta_1 - eta_2)) + ((phi_1 - phi_2)*(phi_1 - phi_2)) );
  
  return dR;
}


std::string get_str(float number)
{
  std::ostringstream ss;
  
  ss << number;
  
  return ss.str();
} 
