/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IForwardRegionProperties_H
#define IForwardRegionProperties_H

#include "GaudiKernel/IAlgTool.h"

#include "CLHEP/Geometry/Point3D.h"

typedef struct _FWDMg_CONFIGURATION
{
    std::string twissFileB1, twissFileB2;
    double momentum;

    std::vector<double> pointQ1Start;
    std::vector<double> pointQ2aStart;
    std::vector<double> pointQ2bStart;
    std::vector<double> pointQ3Start;
    std::vector<double> pointQ4Start;
    std::vector<double> pointQ5Start;
    std::vector<double> pointQ6Start;
    std::vector<double> pointQ7aStart;
    std::vector<double> pointQ7bStart;
    std::vector<double> pointD1aStart;
    std::vector<double> pointD1bStart;
    std::vector<double> pointD1cStart;
    std::vector<double> pointD1dStart;
    std::vector<double> pointD1eStart;
    std::vector<double> pointD1fStart;
    std::vector<double> pointD2Start;

    std::vector<double> pointQ1End;
    std::vector<double> pointQ2aEnd;
    std::vector<double> pointQ2bEnd;
    std::vector<double> pointQ3End;
    std::vector<double> pointQ4End;
    std::vector<double> pointQ5End;
    std::vector<double> pointQ6End;
    std::vector<double> pointQ7aEnd;
    std::vector<double> pointQ7bEnd;
    std::vector<double> pointD1aEnd;
    std::vector<double> pointD1bEnd;
    std::vector<double> pointD1cEnd;
    std::vector<double> pointD1dEnd;
    std::vector<double> pointD1eEnd;
    std::vector<double> pointD1fEnd;
    std::vector<double> pointD2End;

    std::vector<double> fQ1RotZ;
    std::vector<double> fQ2aRotZ;
    std::vector<double> fQ2bRotZ;
    std::vector<double> fQ3RotZ;
    std::vector<double> fQ4RotZ;
    std::vector<double> fQ5RotZ;
    std::vector<double> fQ6RotZ;
    std::vector<double> fQ7aRotZ;
    std::vector<double> fQ7bRotZ;
    std::vector<double> fD1aRotZ;
    std::vector<double> fD1bRotZ;
    std::vector<double> fD1cRotZ;
    std::vector<double> fD1dRotZ;
    std::vector<double> fD1eRotZ;
    std::vector<double> fD1fRotZ;
    std::vector<double> fD2RotZ;

    bool bUseFLUKAMapsForInnerTriplet;



    void clear()
    {
        twissFileB1 = "";
        twissFileB2 = "";
        momentum = 0;

        pointQ1Start.assign(6, 0.0);
        pointQ1End.assign(6, 0.0);
        fQ1RotZ.assign(2, 0.0);

        pointQ2aStart.assign(6, 0.0);
        pointQ2aEnd.assign(6, 0.0);
        fQ2aRotZ.assign(2, 0.0);

        pointQ2bStart.assign(6, 0.0);
        pointQ2bEnd.assign(6, 0.0);
        fQ2bRotZ.assign(2, 0.0);

        pointQ3Start.assign(6, 0.0);
        pointQ3End.assign(6, 0.0);
        fQ3RotZ.assign(2, 0.0);

        pointQ4Start.assign(6, 0.0);
        pointQ4End.assign(6, 0.0);
        fQ4RotZ.assign(2, 0.0);

        pointQ5Start.assign(6, 0.0);
        pointQ5End.assign(6, 0.0);
        fQ5RotZ.assign(2, 0.0);

        pointQ6Start.assign(6, 0.0);
        pointQ6End.assign(6, 0.0);
        fQ6RotZ.assign(2, 0.0);

        pointQ7aStart.assign(6, 0.0);
        pointQ7aEnd.assign(6, 0.0);
        fQ7aRotZ.assign(2, 0.0);

        pointQ7bStart.assign(6, 0.0);
        pointQ7bEnd.assign(6, 0.0);
        fQ7bRotZ.assign(2, 0.0);

        pointD1aStart.assign(6, 0.0);
        pointD1aEnd.assign(6, 0.0);
        fD1aRotZ.assign(2, 0.0);

        pointD1bStart.assign(6, 0.0);
        pointD1bEnd.assign(6, 0.0);
        fD1bRotZ.assign(2, 0.0);

        pointD1cStart.assign(6, 0.0);
        pointD1cEnd.assign(6, 0.0);
        fD1cRotZ.assign(2, 0.0);

        pointD1dStart.assign(6, 0.0);
        pointD1dEnd.assign(6, 0.0);
        fD1dRotZ.assign(2, 0.0);

        pointD1eStart.assign(6, 0.0);
        pointD1eEnd.assign(6, 0.0);
        fD1eRotZ.assign(2, 0.0);

        pointD1fStart.assign(6, 0.0);
        pointD1fEnd.assign(6, 0.0);
        fD1fRotZ.assign(2, 0.0);

        pointD2Start.assign(6, 0.0);
        pointD2End.assign(6, 0.0);
        fD2RotZ.assign(2, 0.0);

        bUseFLUKAMapsForInnerTriplet = false;
    }
} FWDMg_CONFIGURATION, *PFWDMg_CONFIGURATION;

class IForwardRegionProperties : virtual public IAlgTool{

public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IForwardRegionProperties, 1, 0);

  virtual PFWDMg_CONFIGURATION getConf()=0;
  virtual void getMagTransforms(std::string, int, HepGeom::Point3D<double>&, HepGeom::Point3D<double>&, double&, bool twiss = false)=0;
  virtual void getMagTransforms(std::string, int, HepGeom::Point3D<double>&, HepGeom::Point3D<double>&, bool twiss = false)=0;
};

#endif
