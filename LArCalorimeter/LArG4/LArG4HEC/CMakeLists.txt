################################################################################
# Package: LArG4HEC
################################################################################

# Declare the package name:
atlas_subdir( LArG4HEC )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4HEC
                   src/*.cc
                   src/components/*.cxx
                   PUBLIC_HEADERS LArG4HEC
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps LArG4Code LArReadoutGeometry CaloG4SimLib GeoModelInterfaces StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel GaudiKernel LArG4RunControl LArHV RDBAccessSvcLib )

atlas_install_python_modules( python/*.py )
