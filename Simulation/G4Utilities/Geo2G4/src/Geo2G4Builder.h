/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4Builder_H
#define GEO2G4_Geo2G4Builder_H

// main builder to create/position all volumes described in a GeoModel Tree

// GeoVPhysVol
#include "VolumeBuilder.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "G4LogicalVolume.hh"
//#include "Geo2G4/GenericVolumeBuilder.h"

// Typedef
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"

#include "AthenaKernel/MsgStreamMember.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// STL includes
#include <string>
#include <vector>

class GeoMaterial;
class StoreGateSvc;

class Geo2G4Builder {

public:
  // Constructor:
  Geo2G4Builder(const std::string& detectorName);
  // Destructor:
  ~Geo2G4Builder() {;}

  // Build method - geometry
  G4LogicalVolume*        BuildTree();

  // Build method - optical surfaces
  void BuildOpticalSurfaces(const GeoBorderSurfaceContainer* surface_container,
                            const OpticalVolumesMap* optical_volumes);

  // Access volume builder:
  VolumeBuilder*        GetVolumeBuilder(std::string);

  HepGeom::Transform3D GetDetectorTransform() {return Amg::EigenTransformToCLHEP(m_motherTransform);}
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

private:

  // GeoVDetectorManager* theDetectorElement;
  std::string m_detectorName;
  GeoTrf::Transform3D m_motherTransform;
  std::vector<PVConstLink> m_treeTops;
  VolumeBuilder *m_theBuilder;

  // std::Air in the case when top boolean envelope has to be built
  const GeoMaterial* m_matAir;
  StoreGateSvc* m_pDetStore;

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};

#endif
