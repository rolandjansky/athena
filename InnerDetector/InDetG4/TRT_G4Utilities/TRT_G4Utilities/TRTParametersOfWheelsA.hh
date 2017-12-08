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

    int printMessages;

    int numberOfWheelsA;
    double distanceBetweenWheelsA;
    double* positionsOfWheelsA;

    double innerRadiusOfWheelA;
    double outerRadiusOfWheelA;
    double lengthOfWheelA;

    double innerRadiusOfInnerSupportA;
    double outerRadiusOfInnerSupportA;
    double lengthOfInnerSupportA;

    double innerRadiusOfOuterSupportA;
    double outerRadiusOfOuterSupportA;
    double lengthOfOuterSupportA;

    int numberOfStrawPlanesA;
    int numberOfStrawsInPlaneA;
    double* positionsOfStrawPlanesA;
    double* rotationAnglesOfStrawPlanesA;

    int numberOfMainRadiatorsA;
    double* positionsOfMainRadiatorsA;

    double innerRadiusOfMainRadiatorA;
    double outerRadiusOfMainRadiatorA;
    double lengthOfMainRadiatorA;

    int numberOfThinRadiatorsA;
    double* positionsOfThinRadiatorsA;

    double innerRadiusOfThinRadiatorA;
    double outerRadiusOfThinRadiatorA;
    double lengthOfThinRadiatorA;

    TRTParameters* pParameters;
  
    mutable Athena::MsgStreamMember m_msg;
};

#endif
