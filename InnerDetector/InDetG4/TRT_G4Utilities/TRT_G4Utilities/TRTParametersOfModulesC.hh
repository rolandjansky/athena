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
  
  int printMessages;
  
  double baseOfShellTrd1C;
  double heightOfShellTrd1C;
  
  double baseOfShellTrd2C;
  double heightOfShellTrd2C;
  
  double xOfShellTrd2C;
  double zOfShellTrd2C;
  double phiOfShellTrd2C;
  
  double x0OfShellC;
  double y0OfShellC;
  double phi0OfShellC;
  
  int numberOfModulesC;
  double lengthOfModuleC;
  
  double baseOfRadiatorTrd1C;
  double heightOfRadiatorTrd1C;
  
  double baseOfRadiatorTrd2C;
  double heightOfRadiatorTrd2C;
  
  double xOfRadiatorTrd2C;
  double zOfRadiatorTrd2C;
  double phiOfRadiatorTrd2C;
  
  double xOfRadiatorC;
  double zOfRadiatorC;
  
  int numberOfHolesC;
  
  double* xLocalOfHolesC;
  double* zLocalOfHolesC;
  
  int numberOfCoolingTubesC;
  
  double* xOfCoolingTubesC;
  double* zOfCoolingTubesC;
  
  double radiusOfHoleForCoolingTubeC;

  double* xOfHolesForCoolingTubesC;
  double* zOfHolesForCoolingTubesC;
  
  TRTParameters* pParameters;

  mutable Athena::MsgStreamMember m_msg;

};

#endif
