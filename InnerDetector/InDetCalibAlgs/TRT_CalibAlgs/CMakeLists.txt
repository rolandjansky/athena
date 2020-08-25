################################################################################
# Package: TRT_CalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( TRT_CalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetRecTools/TRT_ElectronPidTools
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthContainers
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetCalibEvent/TRT_CalibData
                          InnerDetector/InDetCalibTools/TRT_CalibTools
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_TrackHoleSearch
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TRT_CalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests xAODEventInfo GaudiKernel TrkToolInterfaces CommissionEvent AthContainers Identifier xAODTracking TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier InDetRawData InDetPrepRawData InDetRIO_OnTrack TrkParameters TrkTrack VxVertex TrkFitterInterfaces TRT_TrackHoleSearchLib TRT_ElectronPidToolsLib TRT_CalibToolsLib )

# Install files from the package:
atlas_install_headers( TRT_CalibAlgs )
atlas_install_joboptions( share/*.py )

