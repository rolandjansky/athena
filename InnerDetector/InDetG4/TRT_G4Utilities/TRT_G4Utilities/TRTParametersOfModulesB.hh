/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfModulesB_hh
#define TRTParametersOfModulesB_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters;


class TRTParametersOfModulesB
{
  friend class TRTConstructionOfModulesB;

  public:
    TRTParametersOfModulesB();
    ~TRTParametersOfModulesB();
   
     MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
     bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfModulesB (const TRTParametersOfModulesB&); 
    TRTParametersOfModulesB& operator= (const TRTParametersOfModulesB&); 
    void DefineParameters();
    void PrintParameters(MsgStream& msg, double*, double*) const;

    double m_baseOfShellTrd1B = 0.0;
    double m_heightOfShellTrd1B = 0.0;

    double m_baseOfShellTrd2B = 0.0;
    double m_heightOfShellTrd2B = 0.0;

    double m_xOfShellTrd2B = 0.0;
    double m_zOfShellTrd2B = 0.0;
    double m_phiOfShellTrd2B = 0.0;

    double m_x0OfShellB = 0.0;
    double m_y0OfShellB = 0.0;
    double m_phi0OfShellB = 0.0;

    int m_numberOfModulesB = 0;
    double m_lengthOfModuleB = 0.0;

    double m_baseOfRadiatorTrd1B = 0.0;
    double m_heightOfRadiatorTrd1B = 0.0;

    double m_baseOfRadiatorTrd2B = 0.0;
    double m_heightOfRadiatorTrd2B = 0.0;

    double m_xOfRadiatorTrd2B = 0.0;
    double m_zOfRadiatorTrd2B = 0.0;
    double m_phiOfRadiatorTrd2B = 0.0;

    double m_xOfRadiatorB = 0.0;
    double m_zOfRadiatorB = 0.0;

    int m_numberOfHolesB = 0;

    double* m_xLocalOfHolesB = nullptr;
    double* m_zLocalOfHolesB = nullptr;

    int m_numberOfCoolingTubesB = 0;

    double* m_xOfCoolingTubesB = nullptr;
    double* m_zOfCoolingTubesB = nullptr;

    double m_radiusOfHoleForCoolingTubeB = 0.0;

    double* m_xOfHolesForCoolingTubesB = nullptr;
    double* m_zOfHolesForCoolingTubesB = nullptr;

    const TRTParameters* m_pParameters;

    Athena::MsgStreamMember m_msg;
};

#endif
