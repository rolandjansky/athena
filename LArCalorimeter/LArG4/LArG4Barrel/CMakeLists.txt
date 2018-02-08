################################################################################
# Package: LArG4Barrel
################################################################################

# Declare the package name:
atlas_subdir( LArG4Barrel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloG4Sim
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4Code
                          LArCalorimeter/LArGeoModel/LArHV
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4Barrel
                   src/*.cxx src/components/*.cxx
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} LArG4Code CaloG4SimLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaKernel CxxUtils GaudiKernel LArG4RunControl LArHV PathResolver )

atlas_install_python_modules( python/*.py )
