# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1GeometrySystems )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets Sql )
find_package( GeoModelCore )
if( APPLE )
  find_package(OpenGL REQUIRED)
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Generate UI files automatically:
# Note: add the "Widgets" component to "find_package( Qt5 ...)" if you have UI files, otherwise UIC, even if CMAKE_AUTOUIC is set to ON, is not run
set( CMAKE_AUTOUIC ON ) 
# Generate MOC files automatically:
set( CMAKE_AUTOMOC ON )
# Generate resource files automatically:
set( CMAKE_AUTORCC ON )

### Build the library. 
# Remember: the auto-generated UI files are put under
# CMAKE_CURRENT_BINARY_DIR, so it needs to be added explicitly to the
# declaration of PRIVATE_INCLUDE_DIRS.

if(NOT BUILDVP1LIGHT)
atlas_add_library( VP1GeometrySystems
   VP1GeometrySystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1GeometrySystems
   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}  
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${COIN3D_LIBRARIES} ${GEOMODELCORE_LIBRARIES} MuonReadoutGeometry TruthUtils xAODTruth VP1Base GL Qt5::Core Qt5::Widgets
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelUtilities VP1HEPVis VP1Utils Qt5::Gui )
endif()
if(BUILDVP1LIGHT)
# Files
file(GLOB SOURCES src/DumpShape.cxx
                  src/VP1GeomUtils.cxx
                  src/VolumeTreeModel.cxx
                  src/GeoSysController.cxx
                  src/VP1GeometrySystem.cxx
                  src/ZappedVolumeListModel.cxx
                  src/PhiSectorManager.cxx
                  src/VisAttributes.cxx
                  src/VP1GeoFlags.cxx
                  src/VolumeHandle.cxx
                  src/VP1GeoTreeView.cxx
                  src/VolumeHandleSharedData.cxx
 )
file(GLOB HEADERS VP1GeometrySystems/PhiSectorManager.h
                      VP1GeometrySystems/ZappedVolumeListModel.h
                      VP1GeometrySystems/GeoSysController.h
                      VP1GeometrySystems/VP1GeometrySystem.h
                      VP1GeometrySystems/VP1GeoTreeView.h
                      VP1GeometrySystems/VolumeTreeModel.h
)

if( APPLE )
  atlas_add_library( VP1GeometrySystems
     ${HEADERS} ${SOURCES}
     PUBLIC_HEADERS VP1GeometrySystems
     INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
     PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
     PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
     LINK_LIBRARIES ${COIN3D_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelDBManager GeoRead MuonReadoutGeometry TruthUtils xAODTruth VP1Base ${OPENGL_gl_LIBRARY} Qt5::Core Qt5::Widgets
     PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelUtilities GeoModelExamplesLib VP1HEPVis VP1Utils Qt5::Gui Qt5::Sql )
endif()
if( UNIX AND NOT APPLE )
  atlas_add_library( VP1GeometrySystems
    ${HEADERS} ${SOURCES}
    PUBLIC_HEADERS VP1GeometrySystems
    INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
    PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
    LINK_LIBRARIES ${COIN3D_LIBRARIES} ${GEOMODELCORE_LIBRARIES} GeoModelDBManager GeoRead MuonReadoutGeometry TruthUtils xAODTruth VP1Base GL Qt5::Core Qt5::Widgets
    PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelUtilities GeoModelExamplesLib VP1HEPVis VP1Utils Qt5::Gui Qt5::Sql )
endif()
endif()
