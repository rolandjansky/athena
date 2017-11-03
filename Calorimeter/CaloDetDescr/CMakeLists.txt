################################################################################
# Package: CaloDetDescr
################################################################################

# Declare the package name:
atlas_subdir( CaloDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloConditions
                          Calorimeter/CaloGeoHelpers
                          Control/StoreGate
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CaloDetDescrLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloDetDescr
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives Identifier GaudiKernel LArReadoutGeometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloConditions CaloGeoHelpers )

atlas_add_component( CaloDetDescr
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives Identifier GaudiKernel LArReadoutGeometry CaloConditions CaloGeoHelpers StoreGateLib SGtests CaloDetDescrLib )

atlas_add_dictionary( CaloDetDescrDict
                      CaloDetDescr/CaloDetDescrDict.h
                      CaloDetDescr/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel DataModel SGTools GeoPrimitives Identifier GaudiKernel LArReadoutGeometry CaloConditions CaloGeoHelpers StoreGateLib SGtests CaloDetDescrLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( CaloConstIteratorAdaptor_test
  SOURCES test/CaloConstIteratorAdaptor_test.cxx
  LINK_LIBRARIES CaloDetDescrLib )
