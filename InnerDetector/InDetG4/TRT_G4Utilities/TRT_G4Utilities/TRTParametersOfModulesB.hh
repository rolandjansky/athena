/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesB_hh
#define TRTParametersOfModulesB_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfModulesB
{
  friend class TRTConstructionOfModulesB;

  public:
    TRTParametersOfModulesB();
    ~TRTParametersOfModulesB();
   
     MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
     bool msgLevel (MSG::Level lvl) const    { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfModulesB (const TRTParametersOfModulesB&); 
    TRTParametersOfModulesB& operator= (const TRTParametersOfModulesB&); 
    void DefineParameters();
    void PrintParameters(double*, double*) const;

    int printMessages;

    double baseOfShellTrd1B;
    double heightOfShellTrd1B;

    double baseOfShellTrd2B;
    double heightOfShellTrd2B;

    double xOfShellTrd2B;
    double zOfShellTrd2B;
    double phiOfShellTrd2B;

    double x0OfShellB;
    double y0OfShellB;
    double phi0OfShellB;

    int numberOfModulesB;
    double lengthOfModuleB;

    double baseOfRadiatorTrd1B;
    double heightOfRadiatorTrd1B;

    double baseOfRadiatorTrd2B;
    double heightOfRadiatorTrd2B;

    double xOfRadiatorTrd2B;
    double zOfRadiatorTrd2B;
    double phiOfRadiatorTrd2B;

    double xOfRadiatorB;
    double zOfRadiatorB;

    int numberOfHolesB;

    double* xLocalOfHolesB;
    double* zLocalOfHolesB;

    int numberOfCoolingTubesB;

    double* xOfCoolingTubesB;
    double* zOfCoolingTubesB;

    double radiusOfHoleForCoolingTubeB;

    double* xOfHolesForCoolingTubesB;
    double* zOfHolesForCoolingTubesB;

    TRTParameters* pParameters;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
