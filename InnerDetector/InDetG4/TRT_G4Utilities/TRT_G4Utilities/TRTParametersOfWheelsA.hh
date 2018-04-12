/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfWheelsA_hh
#define TRTParametersOfWheelsA_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfWheelsA
{
  friend class TRTConstructionOfWheelsA;

  public:
    TRTParametersOfWheelsA();
    ~TRTParametersOfWheelsA();
  
    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const   { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfWheelsA (const TRTParametersOfWheelsA&); 
    TRTParametersOfWheelsA& operator= (const TRTParametersOfWheelsA&); 

    void DefineParameters();
    void PrintParameters() const;

    int m_numberOfWheelsA;
    double m_distanceBetweenWheelsA;
    double* m_positionsOfWheelsA;

    double m_innerRadiusOfWheelA;
    double m_outerRadiusOfWheelA;
    double m_lengthOfWheelA;

    double m_innerRadiusOfInnerSupportA;
    double m_outerRadiusOfInnerSupportA;
    double m_lengthOfInnerSupportA;

    double m_innerRadiusOfOuterSupportA;
    double m_outerRadiusOfOuterSupportA;
    double m_lengthOfOuterSupportA;

    int m_numberOfStrawPlanesA;
    int m_numberOfStrawsInPlaneA;
    double* m_positionsOfStrawPlanesA;
    double* m_rotationAnglesOfStrawPlanesA;

    int m_numberOfMainRadiatorsA;
    double* m_positionsOfMainRadiatorsA;

    double m_innerRadiusOfMainRadiatorA;
    double m_outerRadiusOfMainRadiatorA;
    double m_lengthOfMainRadiatorA;

    int m_numberOfThinRadiatorsA;
    double* m_positionsOfThinRadiatorsA;

    double m_innerRadiusOfThinRadiatorA;
    double m_outerRadiusOfThinRadiatorA;
    double m_lengthOfThinRadiatorA;

    TRTParameters* m_pParameters;
  
    mutable Athena::MsgStreamMember m_msg;
};

#endif
