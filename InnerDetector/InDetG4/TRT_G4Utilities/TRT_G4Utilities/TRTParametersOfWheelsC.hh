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

    int m_numberOfWheelsC;
    double m_distanceBetweenWheelsC;
    double* m_positionsOfWheelsC;

    double m_innerRadiusOfWheelC;
    double m_outerRadiusOfWheelC;
    double m_lengthOfWheelC;

    double m_innerRadiusOfInnerSupportC;
    double m_outerRadiusOfInnerSupportC;
    double m_lengthOfInnerSupportC;

    double m_innerRadiusOfOuterSupportC;
    double m_outerRadiusOfOuterSupportC;
    double m_lengthOfOuterSupportC;

    int m_numberOfStrawPlanesC;
    int m_numberOfStrawsInPlaneC;
    double* m_positionsOfStrawPlanesC;
    double* m_rotationAnglesOfStrawPlanesC;

    int m_numberOfMainRadiatorsC;
    double* m_positionsOfMainRadiatorsC;

    double m_innerRadiusOfMainRadiatorC;
    double m_outerRadiusOfMainRadiatorC;
    double m_lengthOfMainRadiatorC;

    int m_numberOfThinRadiatorsC;
    double* m_positionsOfThinRadiatorsC;

    double m_innerRadiusOfThinRadiatorC;
    double m_outerRadiusOfThinRadiatorC;
    double m_lengthOfThinRadiatorC;

    TRTParameters* m_pParameters;
 
    mutable Athena::MsgStreamMember m_msg;
};

#endif
