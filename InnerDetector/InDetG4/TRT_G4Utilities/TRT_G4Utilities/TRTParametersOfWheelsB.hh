/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfWheelsB_hh
#define TRTParametersOfWheelsB_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfWheelsB
{
  friend class TRTConstructionOfWheelsB;

  public:
    TRTParametersOfWheelsB();
    ~TRTParametersOfWheelsB();

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const   { return m_msg.get().level() <= lvl; }
 
  private:
    TRTParametersOfWheelsB (const TRTParametersOfWheelsB&); 
    TRTParametersOfWheelsB& operator= (const TRTParametersOfWheelsB&); 

    void DefineParameters();
    void PrintParameters() const;

    int printMessages;

    int numberOfWheelsB;
    double distanceBetweenWheelsB;
    double* positionsOfWheelsB;

    double innerRadiusOfWheelB;
    double outerRadiusOfWheelB;
    double lengthOfWheelB;

    double innerRadiusOfInnerSupportB;
    double outerRadiusOfInnerSupportB;
    double lengthOfInnerSupportB;

    double innerRadiusOfOuterSupportB;
    double outerRadiusOfOuterSupportB;
    double lengthOfOuterSupportB;

    int numberOfStrawPlanesB;
    int numberOfStrawsInPlaneB;
    double* positionsOfStrawPlanesB;
    double* rotationAnglesOfStrawPlanesB;

    int numberOfMainRadiatorsB;
    double* positionsOfMainRadiatorsB;

    double innerRadiusOfMainRadiatorB;
    double outerRadiusOfMainRadiatorB;
    double lengthOfMainRadiatorB;

    int numberOfThinRadiatorsB;
    double* positionsOfThinRadiatorsB;

    double innerRadiusOfThinRadiatorB;
    double outerRadiusOfThinRadiatorB;
    double lengthOfThinRadiatorB;

    int numberOfMiddleRadiatorsB;
    double* positionsOfMiddleRadiatorsB;

    double innerRadiusOfMiddleRadiatorB;
    double outerRadiusOfMiddleRadiatorB;
    double lengthOfMiddleRadiatorB;

    TRTParameters* pParameters;
  
    mutable Athena::MsgStreamMember m_msg;
};

#endif
