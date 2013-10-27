/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfWheelsC_hh
#define TRTParametersOfWheelsC_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfWheelsC
{
  friend class TRTConstructionOfWheelsC;

  public:
    TRTParametersOfWheelsC();
    ~TRTParametersOfWheelsC();

     MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
     bool msgLevel (MSG::Level lvl) const   { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfWheelsC (const TRTParametersOfWheelsC&); 
    TRTParametersOfWheelsC& operator= (const TRTParametersOfWheelsC&); 

    void DefineParameters();
    void PrintParameters() const;

    int printMessages;

    int numberOfWheelsC;
    double distanceBetweenWheelsC;
    double* positionsOfWheelsC;

    double innerRadiusOfWheelC;
    double outerRadiusOfWheelC;
    double lengthOfWheelC;

    double innerRadiusOfInnerSupportC;
    double outerRadiusOfInnerSupportC;
    double lengthOfInnerSupportC;

    double innerRadiusOfOuterSupportC;
    double outerRadiusOfOuterSupportC;
    double lengthOfOuterSupportC;

    int numberOfStrawPlanesC;
    int numberOfStrawsInPlaneC;
    double* positionsOfStrawPlanesC;
    double* rotationAnglesOfStrawPlanesC;

    int numberOfMainRadiatorsC;
    double* positionsOfMainRadiatorsC;

    double innerRadiusOfMainRadiatorC;
    double outerRadiusOfMainRadiatorC;
    double lengthOfMainRadiatorC;

    int numberOfThinRadiatorsC;
    double* positionsOfThinRadiatorsC;

    double innerRadiusOfThinRadiatorC;
    double outerRadiusOfThinRadiatorC;
    double lengthOfThinRadiatorC;

    TRTParameters* pParameters;
 
    mutable Athena::MsgStreamMember m_msg;
};

#endif
