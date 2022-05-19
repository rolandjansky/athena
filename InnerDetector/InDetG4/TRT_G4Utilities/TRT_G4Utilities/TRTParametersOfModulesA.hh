/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesA_hh
#define TRTParametersOfModulesA_hh

#include "globals.hh"

class TRTParameters;


class TRTParametersOfModulesA
{
  friend class TRTConstructionOfModulesA;
  
public:
  TRTParametersOfModulesA();
  ~TRTParametersOfModulesA();

private:
  TRTParametersOfModulesA (const TRTParametersOfModulesA&); 
  TRTParametersOfModulesA& operator= (const TRTParametersOfModulesA&); 
  void DefineParameters();
  void PrintParameters(double*, double*) const;
  
  double m_baseOfShellTrd1A = 0.0;
  double m_heightOfShellTrd1A = 0.0;
  
  double m_baseOfShellTrd2A = 0.0;
  double m_heightOfShellTrd2A = 0.0;

  double m_xOfShellTrd2A = 0.0;
  double m_zOfShellTrd2A = 0.0;
  double m_phiOfShellTrd2A = 0.0;
  
  double m_x0OfShellA = 0.0;
  double m_y0OfShellA = 0.0;
  double m_phi0OfShellA = 0.0;
  
  int m_numberOfModulesA = 0;
  double m_lengthOfModuleA = 0.0;
  
  double m_baseOfRadiatorTrd1A = 0.0;
  double m_heightOfRadiatorTrd1A = 0.0;
  
  double m_baseOfRadiatorTrd2A = 0.0;
  double m_heightOfRadiatorTrd2A = 0.0;
  
  double m_xOfRadiatorTrd2A = 0.0;
  double m_zOfRadiatorTrd2A = 0.0;
  double m_phiOfRadiatorTrd2A = 0.0;
  
  double m_xOfRadiatorA = 0.0;
  double m_zOfRadiatorA = 0.0;
  
  int m_numberOfHolesA = 0;

  double* m_xLocalOfHolesA = nullptr;
  double* m_zLocalOfHolesA = nullptr;
  
  int m_numberOfCoolingTubesA = 0;
  
  double* m_xOfCoolingTubesA = nullptr;
  double* m_zOfCoolingTubesA = nullptr;

  int m_numberOfShortStrawsA = 0;
  
  const TRTParameters* m_pParameters;
};

#endif
