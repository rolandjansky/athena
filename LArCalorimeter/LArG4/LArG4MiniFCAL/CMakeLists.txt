################################################################################
# Package: LArG4MiniFCAL
################################################################################

# Declare the package name:
atlas_subdir( LArG4MiniFCAL )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          LArCalorimeter/LArG4/LArG4Code
                          Calorimeter/CaloG4Sim
                          Control/AthenaKernel
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4MiniFCAL
                   src/*.cc
                   src/components/*.cxx
                   PUBLIC_HEADERS LArG4MiniFCAL
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel LArG4Code CaloG4SimLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel Identifier LArG4RunControl )

atlas_install_python_modules( python/*.py )
