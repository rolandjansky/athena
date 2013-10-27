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

    int printMessages;

    double innerRadiusOfStrawPlanesAB;
    double innerRadiusOfStrawPlaneC;
    double outerRadiusOfStrawPlanesAB;
    double outerRadiusOfStrawPlaneC;
    double lengthOfStrawPlane;

    int numberOfSectorsAB;
    int numberOfSectorsC;

    double innerRadiusOfSectorsAB;
    double innerRadiusOfSectorC;
    double outerRadiusOfSectorsAB;
    double outerRadiusOfSectorC;
    double lengthOfSector;

    int numberOfStrawsInPlanesAB;
    int numberOfStrawsInPlaneC;

    int numberOfStrawsInSectorsAB;
    int numberOfStrawsInSectorC;

    double outerRadiusOfStraw;
    double lengthOfStrawsAB;
    double lengthOfStrawC;
    double positionOfStrawsAB;
    double positionOfStrawC;

    double innerRadiusOfGas;
    double outerRadiusOfGas;
    double lengthOfGasAB;
    double lengthOfGasC;

    double innerRadiusOfDeadRegion;
    double outerRadiusOfDeadRegion;
    double lengthOfDeadRegion;
    double positionOfDeadRegionsAB;
    double positionOfDeadRegionC;

    double outerRadiusOfWire;
    double lengthOfWiresAB;
    double lengthOfWireC;

    TRTParameters* pParameters;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
