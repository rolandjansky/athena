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
  
  double m_baseOfShellTrd1A;
  double m_heightOfShellTrd1A;
  
  double m_baseOfShellTrd2A;
  double m_heightOfShellTrd2A;

  double m_xOfShellTrd2A;
  double m_zOfShellTrd2A;
  double m_phiOfShellTrd2A;
  
  double m_x0OfShellA;
  double m_y0OfShellA;
  double m_phi0OfShellA;
  
  int m_numberOfModulesA;
  double m_lengthOfModuleA;
  
  double m_baseOfRadiatorTrd1A;
  double m_heightOfRadiatorTrd1A;
  
  double m_baseOfRadiatorTrd2A;
  double m_heightOfRadiatorTrd2A;
  
  double m_xOfRadiatorTrd2A;
  double m_zOfRadiatorTrd2A;
  double m_phiOfRadiatorTrd2A;
  
  double m_xOfRadiatorA;
  double m_zOfRadiatorA;
  
  int m_numberOfHolesA;

  double* m_xLocalOfHolesA;
  double* m_zLocalOfHolesA;
  
  int m_numberOfCoolingTubesA;
  
  double* m_xOfCoolingTubesA;
  double* m_zOfCoolingTubesA;

  int m_numberOfShortStrawsA;
  
  TRTParameters* m_pParameters;
  
  mutable Athena::MsgStreamMember m_msg;

};

#endif
