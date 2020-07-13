################################################################################
# Package: TRT_TR_Process
################################################################################

# Declare the package name:
atlas_subdir( TRT_TR_Process )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TRT_TR_Process
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests IdDictDetDescr G4AtlasInterfaces GeoMaterial2G4 GeoModelInterfaces PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( data/*.dtd data/*.xml )

