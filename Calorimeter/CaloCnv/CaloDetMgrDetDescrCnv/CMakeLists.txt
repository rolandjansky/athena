################################################################################
# Package: CaloDetMgrDetDescrCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloDetMgrDetDescrCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          TestPolicy
                          TileCalorimeter/TileDetDescr )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( CaloDetMgrDetDescrCnv
                     src/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} DetDescrCnvSvcLib Identifier GaudiKernel CaloDetDescrLib CaloIdentifier AthenaKernel SGTools StoreGateLib SGtests GeoModelUtilities LArReadoutGeometry TileDetDescr )

# Install files from the package:
atlas_install_headers( CaloDetMgrDetDescrCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

