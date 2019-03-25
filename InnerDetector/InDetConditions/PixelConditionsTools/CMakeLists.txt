################################################################################
# Package: PixelConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsTools )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
  # DO NOT ADD A DEPENDENCY ON PixelConditionsServices UNLESS YOU ALSO REMOVE
  # THE DEPENDENCY OF PixelConditionsServices ON THIS PACKAGE!!!!
  set( extra_dep InnerDetector/InDetConditions/InDetConditionsSummaryService InnerDetector/InDetDetDescr/PixelGeoModel InnerDetector/InDetConditions/InDetByteStreamErrors )
set( extra_lib PixelGeoModelLib InDetByteStreamErrors )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
  Control/AthenaKernel
  DetectorDescription/GeoPrimitives
  GaudiKernel
  InnerDetector/InDetConditions/InDetConditionsSummaryService
  PRIVATE
  Control/AthenaBaseComps
  Control/SGTools
  Database/AthenaPOOL/AthenaPoolUtilities
  Database/RegistrationServices
  DetectorDescription/DetDescrCond/DetDescrConditions
  DetectorDescription/Identifier
  DetectorDescription/GeoModel/GeoModelInterfaces
  DetectorDescription/GeoModel/GeoModelUtilities
  InnerDetector/InDetConditions/PixelConditionsData
  InnerDetector/InDetConditions/PixelCoralClientUtils
  InnerDetector/InDetDetDescr/InDetIdentifier
  InnerDetector/InDetDetDescr/InDetReadoutGeometry
  InnerDetector/InDetDetDescr/PixelCabling
  TestPolicy
  Tools/PathResolver
  ${extra_dep} )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( PixelConditionsTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel AthenaBaseComps SGTools AthenaPoolUtilities DetDescrConditions Identifier PixelConditionsData PixelCoralClientUtils InDetIdentifier GeoModelUtilities InDetReadoutGeometry PathResolver ${extra_lib} )

if( NOT SIMULATIONBASE )
  atlas_add_test( PixelConditionsConfig_test
                  SCRIPT test/PixelConditionsConfig_test.py
                  PROPERTIES TIMEOUT 30 )
endif()

# Install files from the package:
atlas_install_headers( PixelConditionsTools )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.txt share/*.py )

