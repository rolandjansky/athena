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

    int printMessages;

    double outerRadiusOfStrawHole;
    double lengthOfStrawHole;

    double outerRadiusOfStraw;
    double lengthOfStraw;

    double innerRadiusOfGas;
    double outerRadiusOfGas;
    double lengthOfGasL;
    double lengthOfGasS;

    double positionOfGasL;
    double positionOfGasS;

    double innerRadiusOfDeadRegion;
    double outerRadiusOfDeadRegion;
    double lengthOfDeadRegion;
    double lengthOfLongDeadRegion;

    double positionOfDeadRegionLA;
    double positionOfDeadRegionLB;
    double positionOfDeadRegionSA;
    double positionOfLongDeadRegionSB;

    double innerRadiusOfTwister;
    double outerRadiusOfTwister;
    double lengthOfTwister;

    double outerRadiusOfWire;
    double lengthOfWire;

    TRTParameters* pParameters;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
