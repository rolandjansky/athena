/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSIMPLEGEOMETRY_H
#define MUONCALIBSIMPLEGEOMETRY_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

    class MuonCalibSurface {
    public:
        virtual ~MuonCalibSurface() = default;
        virtual bool inBounds(const Amg::Vector3D& pos, double tolerance = 0.) const = 0;
    };

    class MuonCalibDisk : public MuonCalibSurface {
    public:
        MuonCalibDisk(double z, double r) : zPos(z), radius(r) {}
        bool inBounds(const Amg::Vector3D& pos, double tolerance = 0.) const;
        double zPos;
        double radius;
    };

    class MuonCalibCylinder : public MuonCalibSurface {
    public:
        MuonCalibCylinder(double r, double l) : radius(r), lenght(l) {}
        bool inBounds(const Amg::Vector3D& pos, double tolerance = 0.) const;
        double radius;
        double lenght;
    };

    class MuonCalibSimpleGeometry {
    public:
        MuonCalibSimpleGeometry() = default;

        const MuonCalibDisk endcapInnerDiskASide{7500., 12000.};
        const MuonCalibDisk endcapMiddleDiskASide{14000., 12000.};
        const MuonCalibDisk endcapOuterDiskASide{21000., 12000.};

        const MuonCalibDisk endcapInnerDiskCSide{-7500., 12000.};
        const MuonCalibDisk endcapMiddleDiskCSide{-14000., 12000.};
        const MuonCalibDisk endcapOuterDiskCSide{-21000., 12000.};

        const MuonCalibCylinder barrelInnerCylinder{5000., 8000.};
        const MuonCalibCylinder barrelMiddleCylinder{7000., 11000.};
        const MuonCalibCylinder barrelOuterCylinder{10000., 13000.};

        const MuonCalibDisk mbtsASide{3560., 890.};
        const MuonCalibDisk mbtsCSide{-3560., 890.};
    };

}  // namespace MuonCalib

#endif
