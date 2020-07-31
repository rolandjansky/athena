################################################################################
# Package: PixelCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( PixelCalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/CxxUtils
   GaudiKernel
   InnerDetector/InDetRecEvent/InDetPrepRawData
   PRIVATE
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/CoralDB
   DetectorDescription/DetDescrCond/DetDescrConditions
   DetectorDescription/Identifier
   Event/EventInfo
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/PixelConditionsData
   InnerDetector/InDetDetDescr/PixelCabling
   InnerDetector/InDetConditions/PixelConditionsTools
   InnerDetector/InDetConditions/InDetByteStreamErrors
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelReadoutGeometry
   InnerDetector/InDetDetDescr/PixelGeoModel
   InnerDetector/InDetRawEvent/InDetRawData
   Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Graf Core Tree MathCore Hist RIO MathMore Physics
   Matrix Gpad )

# Libraries in the package:
atlas_add_library( PixelCalibAlgsLib
   PixelCalibAlgs/*.h src/*.cxx
   PUBLIC_HEADERS PixelCalibAlgs
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps CxxUtils
   GaudiKernel InDetPrepRawData InDetConditionsSummaryService PixelCablingLib
   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolUtilities CoralDB
   DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier
   InDetReadoutGeometry PixelReadoutGeometry InDetRawData PathResolver PixelGeoModelLib PixelConditionsToolsLib CoralDB )

atlas_add_component( PixelCalibAlgs
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel InDetByteStreamErrors PixelConditionsData PixelCalibAlgsLib )

# Executables in the package:
atlas_add_executable( doPixelOfflineCalib
   Application/doPixelOfflineCalib.cxx
   LINK_LIBRARIES PixelCalibAlgsLib )

atlas_add_executable( doPixelOfflineCalibG4
   Application/doPixelOfflineCalibG4.cxx
   LINK_LIBRARIES PixelCalibAlgsLib )

atlas_add_executable( PlotChargeSharingConstants
   Application/PlotChargeSharingConstants.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

atlas_add_executable( PlotPixelClusterOnTrackErrors
   Application/PlotPixelClusterOnTrackErrors.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

atlas_add_executable( updateNoiseMaps
   Application/updateNoiseMaps.C
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

atlas_add_executable( updateDeadMaps
   Application/updateDeadMaps.C
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

atlas_add_executable( createDeadModuleList
   Application/createDeadModuleList.C
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

atlas_add_executable( hackNoiseMapsForStudyCP
   Application/hackNoiseMapsForStudyCP.C
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelConditionsData PixelCalibAlgsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.sh share/*.dat )
