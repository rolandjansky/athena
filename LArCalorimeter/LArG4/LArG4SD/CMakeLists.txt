################################################################################
# Package: LArG4SD
################################################################################

# Declare the package name:
atlas_subdir( LArG4SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloG4Sim
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4Code )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4SD
                     src/*.cc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloG4SimLib GaudiKernel LArG4Code )


atlas_add_test( LArG4SDToolConfig_test
                SCRIPT test/LArG4SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
#atlas_install_headers( LArG4SD )
atlas_install_python_modules( python/*.py )
