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

    int m_numberOfWheelsB;
    double m_distanceBetweenWheelsB;
    double* m_positionsOfWheelsB;

    double m_innerRadiusOfWheelB;
    double m_outerRadiusOfWheelB;
    double m_lengthOfWheelB;

    double m_innerRadiusOfInnerSupportB;
    double m_outerRadiusOfInnerSupportB;
    double m_lengthOfInnerSupportB;

    double m_innerRadiusOfOuterSupportB;
    double m_outerRadiusOfOuterSupportB;
    double m_lengthOfOuterSupportB;

    int m_numberOfStrawPlanesB;
    int m_numberOfStrawsInPlaneB;
    double* m_positionsOfStrawPlanesB;
    double* m_rotationAnglesOfStrawPlanesB;

    int m_numberOfMainRadiatorsB;
    double* m_positionsOfMainRadiatorsB;

    double m_innerRadiusOfMainRadiatorB;
    double m_outerRadiusOfMainRadiatorB;
    double m_lengthOfMainRadiatorB;

    int m_numberOfThinRadiatorsB;
    double* m_positionsOfThinRadiatorsB;

    double m_innerRadiusOfThinRadiatorB;
    double m_outerRadiusOfThinRadiatorB;
    double m_lengthOfThinRadiatorB;

    int m_numberOfMiddleRadiatorsB;
    double* m_positionsOfMiddleRadiatorsB;

    double m_innerRadiusOfMiddleRadiatorB;
    double m_outerRadiusOfMiddleRadiatorB;
    double m_lengthOfMiddleRadiatorB;

    TRTParameters* m_pParameters;
  
    mutable Athena::MsgStreamMember m_msg;
};

#endif
