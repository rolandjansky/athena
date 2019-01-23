/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_GenericTubeMaker_h
#define InDetGeoModelUtils_GenericTubeMaker_h

#include "InDetGeoModelUtils/TubeVolData.h"
#include <string>

class GeoVPhysVol;
class GeoPhysVol;
class GeoFullPhysVol;
class GeoShape;
class IRDBRecord;

/// Helper class to make general tubes used mainly for services.
/// Takes as input a DB table which has the following columns
/// Handles numberouse cases
/// Simple tubes: Rmin2, Rmax2 <=0.
/// Cones: Uses Rmin2 and Rmax2.
/// Tube and cone sectors: Uses phi start and phi delta.
/// If RadialDiv > 0 then simulates the CLHEP::radial dependence of tubes/cables going outward CLHEP::radially.
/// 

namespace InDetDD {

class GenericTubeMaker 
{
public:
  GenericTubeMaker(const IRDBRecord *);
  const TubeVolData & volData() const {return m_volData;}
  std::string name() const;
  std::string materialName() const;
  const GeoShape * buildShape();
  void placeVolume(GeoPhysVol * parent, GeoVPhysVol * child, double zParent = 0);
  void placeVolume(GeoFullPhysVol * fullparent, GeoVPhysVol * child, double zParent = 0);
  void placeVolTwoSide(GeoPhysVol * parentPos, GeoPhysVol * parentNeg, GeoVPhysVol * child, double zParent = 0);
  void placeVolTwoSide(GeoFullPhysVol * fullparentPos, GeoFullPhysVol * fullparentNeg, GeoVPhysVol * child, double zParent = 0);


private:

  void placeVolume(GeoPhysVol * parent, GeoFullPhysVol * fullparent, GeoVPhysVol * child, double zParent);
  void placeVolTwoSide(GeoPhysVol * parentPos, GeoPhysVol * parentNeg, 
		       GeoFullPhysVol * fullparentPos, GeoFullPhysVol * fullparentNeg, 
		       GeoVPhysVol * child, double zParent);

  const IRDBRecord * m_record;
  TubeVolData m_volData;  
};

} // end namespace

#endif // InDetGeoModelUtils_GenericTubeMaker_h
