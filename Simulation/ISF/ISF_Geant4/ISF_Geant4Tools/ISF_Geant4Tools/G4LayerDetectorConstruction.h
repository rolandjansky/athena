/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4LayerDetectorConstruction.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_G4LAYERDETECTORCONSTRUCTION_H
#define ISF_GEANT4TOOLS_G4LAYERDETECTORCONSTRUCTION_H 1

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;

// Geant4
#include "G4VUserDetectorConstruction.hh"
// Trk
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class MaterialProperties;
}
/** @class G4LayerDetectorConstruction

    @author: Andreas.Salzburger@cern.ch
*/
class G4LayerDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  G4LayerDetectorConstruction();

  ~G4LayerDetectorConstruction();

  void updateDetector(const Amg::Vector3D& momentum,
                      const Trk::MaterialProperties& emp,
                      double pathCorrection = 1.);

  G4VPhysicalVolume* Construct();

private:

  // Material
  G4Material*               m_layerMaterial;

  // Logical volumes
  //
  G4LogicalVolume*          m_worldLog;
  G4LogicalVolume*          m_detectorLog;
  G4LogicalVolume*          m_layerLog;

  // Physical volumes
  //
  G4VPhysicalVolume*        m_worldPhys;
  G4VPhysicalVolume*        m_detectorPhys;
  G4VPhysicalVolume*        m_layerPhys;

  mutable const Amg::Vector3D*            m_momentum;
  mutable const Trk::MaterialProperties* m_ematProp;

  // Vaccum material
  //
  static G4Material*        s_g4vacuum;

};

#endif
