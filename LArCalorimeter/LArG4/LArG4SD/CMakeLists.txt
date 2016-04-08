################################################################################
# Package: LArG4SD
################################################################################

# Declare the package name:
atlas_subdir( LArG4SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloSimEvent
                          Control/CxxUtils
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4Barrel
                          LArCalorimeter/LArG4/LArG4Code
                          LArCalorimeter/LArG4/LArG4EC
                          LArCalorimeter/LArG4/LArG4FCAL
                          LArCalorimeter/LArG4/LArG4HEC
                          LArCalorimeter/LArG4/LArG4MiniFCAL
                          LArCalorimeter/LArSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4SD
                     src/*.cc
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloG4SimLib CaloSimEvent CxxUtils StoreGateLib SGtests GaudiKernel LArG4Barrel LArG4Code LArG4EC LArG4FCAL LArG4HEC LArG4MiniFCAL LArSimEvent )

# Install files from the package:
atlas_install_headers( LArG4SD )
atlas_install_python_modules( python/*.py )

