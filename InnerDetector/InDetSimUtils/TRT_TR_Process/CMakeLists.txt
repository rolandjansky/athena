################################################################################
# Package: TRT_TR_Process
################################################################################

# Declare the package name:
atlas_subdir( TRT_TR_Process )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/IdDictDetDescr
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Sim/SimHelpers
                          Simulation/G4Utilities/GeoMaterial2G4
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TRT_TR_Process
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests IdDictDetDescr G4AtlasInterfaces SimHelpers GeoMaterial2G4 PathResolver )

# Install files from the package:
atlas_install_headers( TRT_TR_Process )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( data/*.dtd data/*.xml )

