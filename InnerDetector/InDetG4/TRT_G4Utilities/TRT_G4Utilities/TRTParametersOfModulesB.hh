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

    double m_baseOfShellTrd1B;
    double m_heightOfShellTrd1B;

    double m_baseOfShellTrd2B;
    double m_heightOfShellTrd2B;

    double m_xOfShellTrd2B;
    double m_zOfShellTrd2B;
    double m_phiOfShellTrd2B;

    double m_x0OfShellB;
    double m_y0OfShellB;
    double m_phi0OfShellB;

    int m_numberOfModulesB;
    double m_lengthOfModuleB;

    double m_baseOfRadiatorTrd1B;
    double m_heightOfRadiatorTrd1B;

    double m_baseOfRadiatorTrd2B;
    double m_heightOfRadiatorTrd2B;

    double m_xOfRadiatorTrd2B;
    double m_zOfRadiatorTrd2B;
    double m_phiOfRadiatorTrd2B;

    double m_xOfRadiatorB;
    double m_zOfRadiatorB;

    int m_numberOfHolesB;

    double* m_xLocalOfHolesB;
    double* m_zLocalOfHolesB;

    int m_numberOfCoolingTubesB;

    double* m_xOfCoolingTubesB;
    double* m_zOfCoolingTubesB;

    double m_radiusOfHoleForCoolingTubeB;

    double* m_xOfHolesForCoolingTubesB;
    double* m_zOfHolesForCoolingTubesB;

    const TRTParameters* m_pParameters;

    Athena::MsgStreamMember m_msg;
};

#endif
