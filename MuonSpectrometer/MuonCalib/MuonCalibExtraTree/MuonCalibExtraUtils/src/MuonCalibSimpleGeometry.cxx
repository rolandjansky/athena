/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonCalibSimpleGeometry.h"

#include <iostream>

namespace MuonCalib {

  MuonCalibSimpleGeometry::MuonCalibSimpleGeometry() : 
    endcapInnerDiskASide(7500.,12000.),endcapMiddleDiskASide(14000.,12000.),endcapOuterDiskASide(21000.,12000.),
    endcapInnerDiskCSide(-7500.,12000.),endcapMiddleDiskCSide(-14000.,12000.),endcapOuterDiskCSide(-21000.,12000.),
    barrelInnerCylinder(5000.,8000.),barrelMiddleCylinder(7000.,11000.),barrelOuterCylinder(10000.,13000.),
    mbtsASide(3560.,890.),mbtsCSide(-3560.,890.)
  {
    
  }

  bool MuonCalibDisk::inBounds( const Amg::Vector3D& pos, double tolerance ) const {

//     std::cout << " Disk: first " << pos.z() << " bound " << zPos 
// 	      << "   second " << pos.perp() << " bound " << radius
// 	      << " tolerance " << tolerance << std::endl;

    // check whether at same z
    if( fabs(pos.z() - zPos) > 0.00001 ) return false;
    
    // check radius distance
    if( pos.perp() - radius > tolerance ) return false;

    return true;
  }

  bool MuonCalibCylinder::inBounds( const Amg::Vector3D& pos, double tolerance ) const {
//     std::cout << " Cylinder: first " << pos.perp() << " bound " << radius 
// 	      << "   second " << pos.z() << " bound " << lenght
// 	      << " tolerance " << tolerance << std::endl;

    // check radius distance
    if( fabs( pos.perp() - radius ) > 0.00001 ) return false;

    // check whether at same z
    if( fabs(pos.z()) - lenght  > tolerance ) return false;
    
    return true;
  }

}
