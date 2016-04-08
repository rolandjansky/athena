################################################################################
# Package: LArReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( LArReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          LArCalorimeter/LArGeoModel/LArHV
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( LArReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS LArReadoutGeometry
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} SGTools GeoModelKernel GeoPrimitives Identifier LArHV StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} GeoModelUtilities GaudiKernel )

