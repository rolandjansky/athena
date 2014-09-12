/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDDetectorPositioner.h"
#include "AGDD2Geo/AGDDPositionerStore.h"
#include "AGDD2Geo/AGDDVolume.h"
#include "AGDD2Geo/AGDDVolumeStore.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

AGDDDetectorPositioner::AGDDDetectorPositioner(std::string n,HepGeom::Transform3D t):AGDDPositioner(n,t)
{
}
AGDDDetectorPositioner::AGDDDetectorPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v):AGDDPositioner(n,r,v)
{
}

