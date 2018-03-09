/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesC_hh
#define TRTParametersOfModulesC_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfModulesC
{
  friend class TRTConstructionOfModulesC;

public:
  TRTParametersOfModulesC();
  ~TRTParametersOfModulesC();
  
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl) const    { return m_msg.get().level() <= lvl; }
  
private:
  TRTParametersOfModulesC (const TRTParametersOfModulesC&); 
  TRTParametersOfModulesC& operator= (const TRTParametersOfModulesC&); 
  void DefineParameters();
  void PrintParameters(double*, double*) const;
  
  double m_baseOfShellTrd1C;
  double m_heightOfShellTrd1C;
  
  double m_baseOfShellTrd2C;
  double m_heightOfShellTrd2C;
  
  double m_xOfShellTrd2C;
  double m_zOfShellTrd2C;
  double m_phiOfShellTrd2C;
  
  double m_x0OfShellC;
  double m_y0OfShellC;
  double m_phi0OfShellC;
  
  int m_numberOfModulesC;
  double m_lengthOfModuleC;
  
  double m_baseOfRadiatorTrd1C;
  double m_heightOfRadiatorTrd1C;
  
  double m_baseOfRadiatorTrd2C;
  double m_heightOfRadiatorTrd2C;
  
  double m_xOfRadiatorTrd2C;
  double m_zOfRadiatorTrd2C;
  double m_phiOfRadiatorTrd2C;
  
  double m_xOfRadiatorC;
  double m_zOfRadiatorC;
  
  int m_numberOfHolesC;
  
  double* m_xLocalOfHolesC;
  double* m_zLocalOfHolesC;
  
  int m_numberOfCoolingTubesC;
  
  double* m_xOfCoolingTubesC;
  double* m_zOfCoolingTubesC;
  
  double m_radiusOfHoleForCoolingTubeC;

  double* m_xOfHolesForCoolingTubesC;
  double* m_zOfHolesForCoolingTubesC;
  
  TRTParameters* m_pParameters;

  mutable Athena::MsgStreamMember m_msg;

};

#endif
