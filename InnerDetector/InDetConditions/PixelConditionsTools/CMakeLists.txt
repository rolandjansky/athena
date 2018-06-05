################################################################################
# Package: PixelConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsTools )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_dep InnerDetector/InDetConditions/InDetConditionsSummaryService InnerDetector/InDetConditions/PixelConditionsServices InnerDetector/InDetDetDescr/PixelGeoModel )
set( extra_lib PixelGeoModelLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
  Control/AthenaKernel
  DetectorDescription/GeoPrimitives
  GaudiKernel
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

# Install files from the package:
atlas_install_headers( PixelConditionsTools )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.txt share/*.py )

