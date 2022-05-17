/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesC_hh
#define TRTParametersOfModulesC_hh

#include "globals.hh"
#include <vector>

class TRTParameters;


class TRTParametersOfModulesC
{
  friend class TRTConstructionOfModulesC;

public:
  TRTParametersOfModulesC();
  ~TRTParametersOfModulesC() = default;
  
private:
  TRTParametersOfModulesC (const TRTParametersOfModulesC&); 
  TRTParametersOfModulesC& operator= (const TRTParametersOfModulesC&); 
  void DefineParameters();
  void PrintParameters(const std::vector<double> &, const std::vector<double> &) const;

  double m_baseOfShellTrd1C = 0.0;
  double m_heightOfShellTrd1C = 0.0;
  
  double m_baseOfShellTrd2C = 0.0;
  double m_heightOfShellTrd2C = 0.0;
  
  double m_xOfShellTrd2C = 0.0;
  double m_zOfShellTrd2C = 0.0;
  double m_phiOfShellTrd2C = 0.0;
  
  double m_x0OfShellC = 0.0;
  double m_y0OfShellC = 0.0;
  double m_phi0OfShellC = 0.0;
  
  int m_numberOfModulesC = 0;
  double m_lengthOfModuleC = 0.0;
  
  double m_baseOfRadiatorTrd1C = 0.0;
  double m_heightOfRadiatorTrd1C = 0.0;
  
  double m_baseOfRadiatorTrd2C = 0.0;
  double m_heightOfRadiatorTrd2C = 0.0;
  
  double m_xOfRadiatorTrd2C = 0.0;
  double m_zOfRadiatorTrd2C = 0.0;
  double m_phiOfRadiatorTrd2C = 0.0;
  
  double m_xOfRadiatorC = 0.0;
  double m_zOfRadiatorC = 0.0;
  
  int m_numberOfHolesC = 0;
  
  std::vector<double> m_xLocalOfHolesC ;
  std::vector<double> m_zLocalOfHolesC ;
  
  int m_numberOfCoolingTubesC = 0;
  
  std::vector<double> m_xOfCoolingTubesC;
  std::vector<double> m_zOfCoolingTubesC;
  
  double m_radiusOfHoleForCoolingTubeC = 0.0;

  std::vector<double> m_xOfHolesForCoolingTubesC ;
  std::vector<double> m_zOfHolesForCoolingTubesC ;
  
  const TRTParameters* m_pParameters;

};

#endif
