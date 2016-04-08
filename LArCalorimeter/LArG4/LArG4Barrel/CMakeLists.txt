################################################################################
# Package: LArG4Barrel
################################################################################

# Declare the package name:
atlas_subdir( LArG4Barrel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloG4Sim
                          LArCalorimeter/LArG4/LArG4Code
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4RunControl
                          LArCalorimeter/LArGeoModel/LArHV
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( LArG4Barrel
                   src/*.cc
                   PUBLIC_HEADERS LArG4Barrel
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} LArG4Code CaloG4SimLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaKernel CxxUtils GaudiKernel LArG4RunControl LArHV PathResolver )

