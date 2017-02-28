################################################################################
# Package: InDetTrigPrepRawDataFormat
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigPrepRawDataFormat )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IRegionSelector
                          Event/ByteStreamCnvSvcBase
                          Event/EventContainers
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          InnerDetector/InDetRecTools/TRT_DriftCircleTool
                          InnerDetector/InDetTrigRecTools/InDetTrigToolInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( InDetTrigPrepRawDataFormat
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES Identifier GaudiKernel InDetPrepRawData TrigInterfacesLib AthenaKernel StoreGateLib SGtests AtlasDetDescr IRegionSelector ByteStreamCnvSvcBaseLib TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry InDetRawData SiClusterizationToolLib TrkTrack TrigSteeringEvent FTK_DataProviderInterfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( InDetTrigPrepRawDataFormat )
atlas_install_python_modules( python/*.py )

