################################################################################
# Package: LArDetDescr
################################################################################

# Declare the package name:
atlas_subdir( LArDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( LArDetDescr
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloGeoHelpers AthenaBaseComps SGTools GeoModelKernel Identifier GaudiKernel LArReadoutGeometry CaloIdentifier StoreGateLib SGtests GeoModelUtilities GeoPrimitives )

# Install files from the package:
atlas_install_headers( LArDetDescr )
atlas_install_joboptions( share/*.py )

