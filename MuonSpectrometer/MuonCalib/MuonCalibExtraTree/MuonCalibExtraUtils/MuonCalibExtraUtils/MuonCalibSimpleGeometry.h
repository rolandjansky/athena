/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSIMPLEGEOMETRY_H
#define MUONCALIBSIMPLEGEOMETRY_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

  class MuonCalibSurface {
  public:
    virtual ~MuonCalibSurface() {}
    virtual bool inBounds( const Amg::Vector3D& pos, double tolerance = 0. ) const = 0;
  };

  class MuonCalibDisk : public MuonCalibSurface {
  public:
    MuonCalibDisk( double z, double r ) : zPos(z), radius(r) {}
    bool inBounds( const Amg::Vector3D& pos, double tolerance = 0. ) const;
    double zPos;
    double radius;
  };

  class MuonCalibCylinder : public MuonCalibSurface {
  public:
    MuonCalibCylinder( double r, double l ) : radius(r), lenght(l) {}
    bool inBounds( const Amg::Vector3D& pos, double tolerance = 0.  ) const;
    double radius;
    double lenght;
  };

  class MuonCalibSimpleGeometry {
  public:
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
