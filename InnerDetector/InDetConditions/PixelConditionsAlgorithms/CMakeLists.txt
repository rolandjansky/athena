################################################################################
# Package: PixelConditionsAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsAlgorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Commission/CommissionEvent
   Control/AthenaBaseComps
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolUtilities
   GaudiKernel
   InnerDetector/InDetConditions/InDetByteStreamErrors
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/PixelConditionsData
   InnerDetector/InDetConditions/PixelConditionsTools
   PRIVATE
   Control/SGTools
   Control/StoreGate
   Database/RDBAccessSvc
   Database/CoralDB
   DetectorDescription/GeoModel/GeoModelInterfaces
   DetectorDescription/GeoModel/GeoModelUtilities
   DetectorDescription/Identifier
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelCabling
   Tools/PathResolver
   Tracking/TrkDetDescr/TrkGeometry )


# Component(s) in the package:
atlas_add_library( PixelConditionsAlgorithmsLib
   PixelConditionsAlgorithms/*.h
   INTERFACE
   NO_PUBLIC_HEADERS
   LINK_LIBRARIES GaudiKernel AthenaKernel AthenaBaseComps PixelConditionsData
   AthenaPoolUtilities PathResolver TrkGeometry)



atlas_add_component( PixelConditionsAlgorithms
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES}
   ${CLHEP_LIBRARIES} CommissionEvent AthenaBaseComps AthenaKernel AthenaPoolUtilities
   GaudiKernel PixelConditionsData SGTools StoreGateLib CoralDB
   GeoModelUtilities Identifier InDetIdentifier InDetReadoutGeometry
   PixelCablingLib PixelConditionsAlgorithmsLib PathResolver )




# Install files from the package:
atlas_install_python_modules( python/*.py )

