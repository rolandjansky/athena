# $Id: CMakeLists.txt 729385 2016-03-13 14:47:24Z krasznaa $
################################################################################
# Package: G4PhysicsLists
################################################################################

# Declare the package name:
atlas_subdir( G4PhysicsLists )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )

# Component(s) in the package:
atlas_add_library( G4PhysicsLists
  src/*.cc src/MuBias/*.cc src/hadronicVariations/*.cc
  PUBLIC_HEADERS G4PhysicsLists
  INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MuBias
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hadronicVariations
  LINK_LIBRARIES ${GEANT4_LIBRARIES}
  PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel )

# Install files from the package:
atlas_install_runtime( share/biasValues.txt )
