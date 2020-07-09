################################################################################
# Package: PixelConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
  Control/AthenaKernel
  DetectorDescription/GeoPrimitives
  GaudiKernel
  InnerDetector/InDetConditions/InDetConditionsSummaryService
  PRIVATE
  Control/AthenaBaseComps
  Control/CxxUtils
  Control/SGTools
  Database/AthenaPOOL/AthenaPoolUtilities
  Database/RegistrationServices
  DetectorDescription/DetDescrCond/DetDescrConditions
  DetectorDescription/Identifier
  DetectorDescription/GeoModel/GeoModelInterfaces
  DetectorDescription/GeoModel/GeoModelUtilities
  InnerDetector/InDetConditions/PixelConditionsData
  InnerDetector/InDetDetDescr/InDetIdentifier
  InnerDetector/InDetDetDescr/InDetReadoutGeometry
  InnerDetector/InDetDetDescr/PixelCabling
  InnerDetector/InDetDetDescr/PixelGeoModel
  InnerDetector/InDetConditions/InDetByteStreamErrors
  Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

atlas_add_library( PixelConditionsToolsLib
                   PixelConditionsTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS PixelConditionsTools
                   LINK_LIBRARIES GaudiKernel AthenaPoolUtilities PixelConditionsData InDetConditionsSummaryService ${interface_extra_lib} )

# Component(s) in the package:
atlas_add_component( PixelConditionsTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} PixelConditionsToolsLib AthenaKernel GeoPrimitives GaudiKernel AthenaBaseComps 
                     CxxUtils SGTools AthenaPoolUtilities DetDescrConditions Identifier PixelCablingLib PixelConditionsData InDetIdentifier 
                     GeoModelUtilities InDetReadoutGeometry PathResolver PixelGeoModelLib InDetByteStreamErrors  )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.txt share/*.py )

