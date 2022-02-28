/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesC_hh
#define TRTParametersOfModulesC_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters;


class TRTParametersOfModulesC
{
  friend class TRTConstructionOfModulesC;

public:
  TRTParametersOfModulesC();
  ~TRTParametersOfModulesC();
  
  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }
  
private:
  TRTParametersOfModulesC (const TRTParametersOfModulesC&); 
  TRTParametersOfModulesC& operator= (const TRTParametersOfModulesC&); 
  void DefineParameters();
  void PrintParameters(MsgStream& msg, double*, double*) const;
  
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
  
  double* m_xLocalOfHolesC = nullptr;
  double* m_zLocalOfHolesC = nullptr;
  
  int m_numberOfCoolingTubesC = 0;
  
  double* m_xOfCoolingTubesC = nullptr;
  double* m_zOfCoolingTubesC = nullptr;
  
  double m_radiusOfHoleForCoolingTubeC = 0.0;

  double* m_xOfHolesForCoolingTubesC = nullptr;
  double* m_zOfHolesForCoolingTubesC = nullptr;
  
  const TRTParameters* m_pParameters;

  Athena::MsgStreamMember m_msg;

};

#endif
