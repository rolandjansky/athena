/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersOfStrawPlanes_hh
#define TRTParametersOfStrawPlanes_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;


class TRTParametersOfStrawPlanes
{
  friend class TRTConstructionOfStrawPlanes;

  public:
    TRTParametersOfStrawPlanes();
    ~TRTParametersOfStrawPlanes();

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const   { return m_msg.get().level() <= lvl; }

  private:
    TRTParametersOfStrawPlanes (const TRTParametersOfStrawPlanes&); 
    TRTParametersOfStrawPlanes& operator= (const TRTParametersOfStrawPlanes&); 
    void DefineParameters();
    void PrintParameters() const;

    double m_innerRadiusOfStrawPlanesAB;
    double m_innerRadiusOfStrawPlaneC;
    double m_outerRadiusOfStrawPlanesAB;
    double m_outerRadiusOfStrawPlaneC;
    double m_lengthOfStrawPlane;

    int m_numberOfSectorsAB;
    int m_numberOfSectorsC;

    double m_innerRadiusOfSectorsAB;
    double m_innerRadiusOfSectorC;
    double m_outerRadiusOfSectorsAB;
    double m_outerRadiusOfSectorC;
    double m_lengthOfSector;

    int m_numberOfStrawsInPlanesAB;
    int m_numberOfStrawsInPlaneC;

    int m_numberOfStrawsInSectorsAB;
    int m_numberOfStrawsInSectorC;

    double m_outerRadiusOfStraw;
    double m_lengthOfStrawsAB;
    double m_lengthOfStrawC;
    double m_positionOfStrawsAB;
    double m_positionOfStrawC;

    double m_innerRadiusOfGas;
    double m_outerRadiusOfGas;
    double m_lengthOfGasAB;
    double m_lengthOfGasC;

    double m_innerRadiusOfDeadRegion;
    double m_outerRadiusOfDeadRegion;
    double m_lengthOfDeadRegion;
    double m_positionOfDeadRegionsAB;
    double m_positionOfDeadRegionC;

    double m_outerRadiusOfWire;
    double m_lengthOfWiresAB;
    double m_lengthOfWireC;

    TRTParameters* m_pParameters;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
