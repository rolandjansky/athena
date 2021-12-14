/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfWheelsC_hh
#define TRTParametersOfWheelsC_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters;


class TRTParametersOfWheelsC
{
  friend class TRTConstructionOfWheelsC;

  public:
    TRTParametersOfWheelsC();
    ~TRTParametersOfWheelsC();

     MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
     bool msgLevel (MSG::Level lvl) { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfWheelsC (const TRTParametersOfWheelsC&); 
    TRTParametersOfWheelsC& operator= (const TRTParametersOfWheelsC&); 

    void DefineParameters();
    void PrintParameters(MsgStream& msg) const;

    int m_numberOfWheelsC = 0;
    double m_distanceBetweenWheelsC = 0.0;
    double* m_positionsOfWheelsC = nullptr;

    double m_innerRadiusOfWheelC = 0.0;
    double m_outerRadiusOfWheelC = 0.0;
    double m_lengthOfWheelC = 0.0;

    double m_innerRadiusOfInnerSupportC = 0.0;
    double m_outerRadiusOfInnerSupportC = 0.0;
    double m_lengthOfInnerSupportC = 0.0;

    double m_innerRadiusOfOuterSupportC = 0.0;
    double m_outerRadiusOfOuterSupportC = 0.0;
    double m_lengthOfOuterSupportC = 0.0;

    int m_numberOfStrawPlanesC = 0;
    int m_numberOfStrawsInPlaneC = 0;
    double* m_positionsOfStrawPlanesC = nullptr;
    double* m_rotationAnglesOfStrawPlanesC = nullptr;

    int m_numberOfMainRadiatorsC = 0;
    double* m_positionsOfMainRadiatorsC = nullptr;

    double m_innerRadiusOfMainRadiatorC = 0.0;
    double m_outerRadiusOfMainRadiatorC = 0.0;
    double m_lengthOfMainRadiatorC = 0.0;

    int m_numberOfThinRadiatorsC = 0;
    double* m_positionsOfThinRadiatorsC = nullptr;

    double m_innerRadiusOfThinRadiatorC = 0.0;
    double m_outerRadiusOfThinRadiatorC = 0.0;
    double m_lengthOfThinRadiatorC = 0.0;

    const TRTParameters* m_pParameters;
 
    Athena::MsgStreamMember m_msg;
};

#endif
