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
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          InnerDetector/InDetRecTools/TRT_DriftCircleTool
                          InnerDetector/InDetTrigRecTools/InDetTrigToolInterfaces
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( InDetTrigPrepRawDataFormat
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES Identifier GaudiKernel InDetPrepRawData TrigInterfacesLib AthenaKernel StoreGateLib SGtests AtlasDetDescr IRegionSelector InDetByteStreamErrors ByteStreamCnvSvcBaseLib TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetRawData SiClusterizationToolLib TrkTrack TrigSteeringEvent TrigTimeAlgsLib PixelConditionsToolsLib TRT_DriftCircleToolLib InDetTrigToolInterfacesLib SCT_ConditionsData )

# Install files from the package:
atlas_install_headers( InDetTrigPrepRawDataFormat )
atlas_install_python_modules( python/*.py )

