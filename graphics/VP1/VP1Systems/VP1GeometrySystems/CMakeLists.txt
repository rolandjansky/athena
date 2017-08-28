# $Id: CMakeLists.txt 732131 2016-03-24 11:03:29Z krasznaa $
################################################################################
# Package: VP1GeometrySystems
################################################################################

# Declare the package name:
atlas_subdir( VP1GeometrySystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/GeoModel/GeoModelKernel
   MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
   Generators/TruthUtils
   Event/xAOD/xAODTruth
   graphics/VP1/VP1Base
   PRIVATE
   DetectorDescription/GeoModel/GeoModelUtilities
   graphics/VP1/VP1HEPVis
   graphics/VP1/VP1Utils )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets  )

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

atlas_add_library( VP1GeometrySystems
   VP1GeometrySystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1GeometrySystems
   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}  
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoModelKernel MuonReadoutGeometry TruthUtils xAODTruth VP1Base GL Qt5::Core Qt5::Widgets
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelUtilities VP1HEPVis VP1Utils Qt5::Gui )
