/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfBarrelStraws_hh
#define TRTParametersOfBarrelStraws_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfBarrelStraws
{
  friend class TRTConstructionOfBarrelStraws;

  public:
    TRTParametersOfBarrelStraws();
    ~TRTParametersOfBarrelStraws();

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const   { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfBarrelStraws (const TRTParametersOfBarrelStraws&); 
    TRTParametersOfBarrelStraws& operator= (const TRTParametersOfBarrelStraws&); 
    void DefineParameters();
    void PrintParameters() const;

    double m_outerRadiusOfStrawHole;
    double m_lengthOfStrawHole;

    double m_outerRadiusOfStraw;
    double m_lengthOfStraw;

    double m_innerRadiusOfGas;
    double m_outerRadiusOfGas;
    double m_lengthOfGasL;
    double m_lengthOfGasS;

    double m_positionOfGasL;
    double m_positionOfGasS;

    double m_innerRadiusOfDeadRegion;
    double m_outerRadiusOfDeadRegion;
    double m_lengthOfDeadRegion;
    double m_lengthOfLongDeadRegion;

    double m_positionOfDeadRegionLA;
    double m_positionOfDeadRegionLB;
    double m_positionOfDeadRegionSA;
    double m_positionOfLongDeadRegionSB;

    double m_innerRadiusOfTwister;
    double m_outerRadiusOfTwister;
    double m_lengthOfTwister;

    double m_outerRadiusOfWire;
    double m_lengthOfWire;

    TRTParameters* m_pParameters;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
