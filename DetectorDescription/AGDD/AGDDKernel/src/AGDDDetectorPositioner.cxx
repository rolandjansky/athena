/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDVolumeStore.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

AGDDDetectorPositioner::AGDDDetectorPositioner(std::string n,HepGeom::Transform3D t):AGDDPositioner(n,t),theDetector(0),theVolume(0)
{
}
AGDDDetectorPositioner::AGDDDetectorPositioner(std::string n,CLHEP::HepRotation r,CLHEP::Hep3Vector v):AGDDPositioner(n,r,v),theDetector(0),theVolume(0)
{
}

