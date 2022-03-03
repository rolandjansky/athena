/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfWheelsB_hh
#define TRTParametersOfWheelsB_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters;


class TRTParametersOfWheelsB
{
  friend class TRTConstructionOfWheelsB;

  public:
    TRTParametersOfWheelsB();
    ~TRTParametersOfWheelsB();

    MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) { return m_msg.get().level() <= lvl; }
 
  private:
    TRTParametersOfWheelsB (const TRTParametersOfWheelsB&); 
    TRTParametersOfWheelsB& operator= (const TRTParametersOfWheelsB&); 

    void DefineParameters();
    void PrintParameters(MsgStream& msg) const;

    int m_numberOfWheelsB = 0;
    double m_distanceBetweenWheelsB = 0.0;
    double* m_positionsOfWheelsB = nullptr;

    double m_innerRadiusOfWheelB = 0.0;
    double m_outerRadiusOfWheelB = 0.0;
    double m_lengthOfWheelB = 0.0;

    double m_innerRadiusOfInnerSupportB = 0.0;
    double m_outerRadiusOfInnerSupportB = 0.0;
    double m_lengthOfInnerSupportB = 0.0;

    double m_innerRadiusOfOuterSupportB = 0.0;
    double m_outerRadiusOfOuterSupportB = 0.0;
    double m_lengthOfOuterSupportB = 0.0;

    int m_numberOfStrawPlanesB = 0;
    int m_numberOfStrawsInPlaneB = 0;
    double* m_positionsOfStrawPlanesB = nullptr;
    double* m_rotationAnglesOfStrawPlanesB = nullptr;

    int m_numberOfMainRadiatorsB = 0;
    double* m_positionsOfMainRadiatorsB = nullptr;

    double m_innerRadiusOfMainRadiatorB = 0.0;
    double m_outerRadiusOfMainRadiatorB = 0.0;
    double m_lengthOfMainRadiatorB = 0.0;

    int m_numberOfThinRadiatorsB = 0;
    double* m_positionsOfThinRadiatorsB = nullptr;

    double m_innerRadiusOfThinRadiatorB = 0.0;
    double m_outerRadiusOfThinRadiatorB = 0.0;
    double m_lengthOfThinRadiatorB = 0.0;

    int m_numberOfMiddleRadiatorsB = 0;
    double* m_positionsOfMiddleRadiatorsB = nullptr;

    double m_innerRadiusOfMiddleRadiatorB = 0.0;
    double m_outerRadiusOfMiddleRadiatorB = 0.0;
    double m_lengthOfMiddleRadiatorB = 0.0;

    const TRTParameters* m_pParameters;
  
    Athena::MsgStreamMember m_msg;
};

#endif
