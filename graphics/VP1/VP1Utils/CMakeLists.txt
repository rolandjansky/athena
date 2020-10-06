# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1Utils )

# External dependencies:
find_package( CLHEP ) # TODO: to be removed when fully migrated to Eigen-based GeoTrf
find_package( Coin3D )
find_package( Eigen )
find_package( HepPDT )
find_package( Qt5 COMPONENTS Core )
find_package( GeoModelCore )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

if (NOT BUILDVP1LIGHT)
# Component(s) in the package:
atlas_add_library( VP1Utils VP1Utils/*.h src/*.cxx src/*.cpp
   PUBLIC_HEADERS VP1Utils
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   ${EIGEN_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} EventPrimitives
   GaudiKernel VP1Base StoreGateLib SGtests AthDSoCallBacks MuonIdHelpersLib
   GeoPrimitives Qt5::Core
   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${COIN3D_LIBRARIES}
   ${EIGEN_LIBRARIES} CaloIdentifier CaloTTDetDescr CxxUtils AtlasDetDescr
   GeoModelUtilities GeoSpecialShapes Identifier InDetIdentifier
   InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry InDetRIO_OnTrack LArReadoutGeometry
   MuonReadoutGeometry MuonRIO_OnTrack TileDetDescr TrkSurfaces
   TrkRIO_OnTrack VP1HEPVis )
endif()
if(BUILDVP1LIGHT)
# Files
file(GLOB SOURCES src/HitsSoNodeManager.cxx
                  src/SoTools.cpp
                  src/InDetProjFlags.cxx
                  src/SoVisualizeAction.cxx
                  src/SbPolyhedrizeAction.cxx
                  src/VP1LinAlgUtils.cxx
)
file(GLOB HEADERS ${module_dir}/HitsSoNodeManager.h
                  ${module_dir}/SoTools.h
                  ${module_dir}/InDetProjFlags.h
                  ${module_dir}/SbPolyhedrizeAction.h
                  ${module_dir}/VP1LinAlgUtils.h
                  ${module_dir}/SoVisualizeAction.h
)

# Component(s) in the package:
atlas_add_library( VP1Utils ${SOURCES} ${HEADERS}
   PUBLIC_HEADERS VP1Utils
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} EventPrimitives ${GEOMODELCORE_LIBRARIES}
   VP1Base
   GeoPrimitives Qt5::Core
   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${COIN3D_LIBRARIES}
   ${EIGEN_LIBRARIES} CxxUtils
   GeoModelUtilities GeoSpecialShapes
   VP1HEPVis )
endif()
