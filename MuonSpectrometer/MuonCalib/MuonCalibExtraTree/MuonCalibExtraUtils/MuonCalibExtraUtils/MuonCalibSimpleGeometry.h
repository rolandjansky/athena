/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSIMPLEGEOMETRY_H
#define MUONCALIBSIMPLEGEOMETRY_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

  struct MuonCalibSurface {
    virtual ~MuonCalibSurface() {}
    virtual bool inBounds( const Amg::Vector3D& pos, double tolerance = 0. ) const = 0;
  };

  struct MuonCalibDisk : public MuonCalibSurface {
    MuonCalibDisk( double z, double r ) : zPos(z), radius(r) {}
    bool inBounds( const Amg::Vector3D& pos, double tolerance = 0. ) const;
    double zPos;
    double radius;
  };

  struct MuonCalibCylinder : public MuonCalibSurface {
    MuonCalibCylinder( double r, double l ) : radius(r), lenght(l) {}
    bool inBounds( const Amg::Vector3D& pos, double tolerance = 0.  ) const;
    double radius;
    double lenght;
  };

  struct MuonCalibSimpleGeometry {
    MuonCalibSimpleGeometry();

    MuonCalibDisk     endcapInnerDiskASide; 
    MuonCalibDisk     endcapMiddleDiskASide; 
    MuonCalibDisk     endcapOuterDiskASide;
    MuonCalibDisk     endcapInnerDiskCSide; 
    MuonCalibDisk     endcapMiddleDiskCSide; 
    MuonCalibDisk     endcapOuterDiskCSide;
    MuonCalibCylinder barrelInnerCylinder; 
    MuonCalibCylinder barrelMiddleCylinder;
    MuonCalibCylinder barrelOuterCylinder;
    MuonCalibDisk     mbtsASide; 
    MuonCalibDisk     mbtsCSide; 
  };

}



#endif
