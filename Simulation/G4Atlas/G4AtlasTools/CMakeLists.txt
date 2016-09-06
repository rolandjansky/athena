################################################################################
# Package: G4AtlasTools
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Control/AthenaBaseComps
                          Simulation/G4Atlas/G4AtlasInterfaces
                          PRIVATE
                          GaudiKernel
                          Simulation/G4Utilities/G4PhysicsLists
                          Simulation/G4Utilities/Geo2G4 )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4AtlasToolsLib
                   src/*.cxx
                   src/*.cc
                   PUBLIC_HEADERS G4AtlasTools
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps G4AtlasInterfaces Geo2G4Lib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel G4PhysicsLists )

atlas_add_component( G4AtlasTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps G4AtlasInterfaces GaudiKernel G4PhysicsLists Geo2G4Lib G4AtlasToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

