/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesA_hh
#define TRTParametersOfModulesA_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfModulesA
{
  friend class TRTConstructionOfModulesA;
  
public:
  TRTParametersOfModulesA();
  ~TRTParametersOfModulesA();

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl) const  { return m_msg.get().level() <= lvl; }
  
private:
  TRTParametersOfModulesA (const TRTParametersOfModulesA&); 
  TRTParametersOfModulesA& operator= (const TRTParametersOfModulesA&); 
  void DefineParameters();
  void PrintParameters(double*, double*) const;
  
  int printMessages;
  
  double baseOfShellTrd1A;
  double heightOfShellTrd1A;
  
  double baseOfShellTrd2A;
  double heightOfShellTrd2A;

  double xOfShellTrd2A;
  double zOfShellTrd2A;
  double phiOfShellTrd2A;
  
  double x0OfShellA;
  double y0OfShellA;
  double phi0OfShellA;
  
  int numberOfModulesA;
  double lengthOfModuleA;
  
  double baseOfRadiatorTrd1A;
  double heightOfRadiatorTrd1A;
  
  double baseOfRadiatorTrd2A;
  double heightOfRadiatorTrd2A;
  
  double xOfRadiatorTrd2A;
  double zOfRadiatorTrd2A;
  double phiOfRadiatorTrd2A;
  
  double xOfRadiatorA;
  double zOfRadiatorA;
  
  int numberOfHolesA;

  double* xLocalOfHolesA;
  double* zLocalOfHolesA;
  
  int numberOfCoolingTubesA;
  
  double* xOfCoolingTubesA;
  double* zOfCoolingTubesA;

  int numberOfShortStrawsA;
  
  TRTParameters* pParameters;
  
  mutable Athena::MsgStreamMember m_msg;

};

#endif
