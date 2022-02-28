/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfWheelsA_hh
#define TRTParametersOfWheelsA_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters;


class TRTParametersOfWheelsA
{
  friend class TRTConstructionOfWheelsA;

  public:
    TRTParametersOfWheelsA();
    ~TRTParametersOfWheelsA();
  
    MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)   { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfWheelsA (const TRTParametersOfWheelsA&); 
    TRTParametersOfWheelsA& operator= (const TRTParametersOfWheelsA&); 

    void DefineParameters();
    void PrintParameters(MsgStream& msg) const;

    int m_numberOfWheelsA = 0;
    double m_distanceBetweenWheelsA = 0.0;
    double* m_positionsOfWheelsA = nullptr;

    double m_innerRadiusOfWheelA = 0.0;
    double m_outerRadiusOfWheelA = 0.0;
    double m_lengthOfWheelA = 0.0;

    double m_innerRadiusOfInnerSupportA = 0.0;
    double m_outerRadiusOfInnerSupportA = 0.0;
    double m_lengthOfInnerSupportA = 0.0;

    double m_innerRadiusOfOuterSupportA = 0.0;
    double m_outerRadiusOfOuterSupportA = 0.0;
    double m_lengthOfOuterSupportA = 0.0;

    int m_numberOfStrawPlanesA = 0;
    int m_numberOfStrawsInPlaneA = 0;
    double* m_positionsOfStrawPlanesA = nullptr;
    double* m_rotationAnglesOfStrawPlanesA = nullptr;

    int m_numberOfMainRadiatorsA = 0;
    double* m_positionsOfMainRadiatorsA = nullptr;

    double m_innerRadiusOfMainRadiatorA = 0.0;
    double m_outerRadiusOfMainRadiatorA = 0.0;
    double m_lengthOfMainRadiatorA = 0.0;

    int m_numberOfThinRadiatorsA = 0;
    double* m_positionsOfThinRadiatorsA = nullptr;

    double m_innerRadiusOfThinRadiatorA = 0.0;
    double m_outerRadiusOfThinRadiatorA = 0.0;
    double m_lengthOfThinRadiatorA = 0.0;

    const TRTParameters* m_pParameters;
  
    Athena::MsgStreamMember m_msg;
};

#endif
