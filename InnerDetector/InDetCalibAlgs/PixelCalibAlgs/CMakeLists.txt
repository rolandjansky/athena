################################################################################
# Package: PixelCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( PixelCalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
			  Control/StoreGate
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
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          InnerDetector/InDetRawEvent/InDetRawData
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Matrix Gpad Graf Core Tree MathCore Hist RIO pthread GenVector MathMore Minuit Minuit2 Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# tag ROOTCintexLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( PixelOfflineCalib
                   src/*.cxx
                   PUBLIC_HEADERS PixelCalibAlgs
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData
		   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver StoreGate )

atlas_add_component( PixelCalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( doPixelOfflineCalib
                      Application/doPixelOfflineCalib.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( doPixelOfflineCalibG4
                      Application/doPixelOfflineCalibG4.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( PlotChargeSharingConstants
                      Application/PlotChargeSharingConstants.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( PlotPixelClusterOnTrackErrors
                      Application/PlotPixelClusterOnTrackErrors.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( updateNoiseMaps
                      Application/updateNoiseMaps.C
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( updateDeadMaps
                      Application/updateDeadMaps.C
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( createDeadModuleList
                      Application/createDeadModuleList.C
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

atlas_add_executable( hackNoiseMapsForStudyCP
                      Application/hackNoiseMapsForStudyCP.C
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetPrepRawData AthenaPoolUtilities CoralDB DetDescrConditions Identifier EventInfo PixelConditionsData InDetIdentifier InDetReadoutGeometry InDetRawData PathResolver PixelOfflineCalib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.sh share/*.dat )

