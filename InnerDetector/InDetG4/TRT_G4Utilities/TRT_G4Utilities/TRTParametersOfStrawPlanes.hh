/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfStrawPlanes_hh
#define TRTParametersOfStrawPlanes_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTParameters;


class TRTParametersOfStrawPlanes
{
  friend class TRTConstructionOfStrawPlanes;

  public:
    TRTParametersOfStrawPlanes();
    ~TRTParametersOfStrawPlanes();

    MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)   { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfStrawPlanes (const TRTParametersOfStrawPlanes&); 
    TRTParametersOfStrawPlanes& operator= (const TRTParametersOfStrawPlanes&); 
    void DefineParameters();
    void PrintParameters(MsgStream& msg) const;

    double m_innerRadiusOfStrawPlanesAB = 0.0;
    double m_innerRadiusOfStrawPlaneC = 0.0;
    double m_outerRadiusOfStrawPlanesAB = 0.0;
    double m_outerRadiusOfStrawPlaneC = 0.0;
    double m_lengthOfStrawPlane = 0.0;

    int m_numberOfSectorsAB = 0;
    int m_numberOfSectorsC = 0;

    double m_innerRadiusOfSectorsAB = 0.0;
    double m_innerRadiusOfSectorC = 0.0;
    double m_outerRadiusOfSectorsAB = 0.0;
    double m_outerRadiusOfSectorC = 0.0;
    double m_lengthOfSector = 0.0;

    int m_numberOfStrawsInPlanesAB = 0;
    int m_numberOfStrawsInPlaneC = 0;

    int m_numberOfStrawsInSectorsAB = 0;
    int m_numberOfStrawsInSectorC = 0;

    double m_outerRadiusOfStraw = 0.0;
    double m_lengthOfStrawsAB = 0.0;
    double m_lengthOfStrawC = 0.0;
    double m_positionOfStrawsAB = 0.0;
    double m_positionOfStrawC = 0.0;

    double m_innerRadiusOfGas = 0.0;
    double m_outerRadiusOfGas = 0.0;
    double m_lengthOfGasAB = 0.0;
    double m_lengthOfGasC = 0.0;

    double m_innerRadiusOfDeadRegion = 0.0;
    double m_outerRadiusOfDeadRegion = 0.0;
    double m_lengthOfDeadRegion = 0.0;
    double m_positionOfDeadRegionsAB = 0.0;
    double m_positionOfDeadRegionC = 0.0;

    double m_outerRadiusOfWire = 0.0;
    double m_lengthOfWiresAB = 0.0;
    double m_lengthOfWireC = 0.0;

    const TRTParameters* m_pParameters;

    Athena::MsgStreamMember m_msg;

};

#endif
