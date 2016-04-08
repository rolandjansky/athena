################################################################################
# Package: TrigOfflineDriftCircleTool
################################################################################

# Declare the package name:
atlas_subdir( TrigOfflineDriftCircleTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkPrepRawData
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          DetectorDescription/IRegionSelector
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/TRT_Cabling
                          InnerDetector/InDetEventCnv/TRT_RawDataByteStreamCnv
                          InnerDetector/InDetRecTools/TRT_DriftCircleTool )

# Component(s) in the package:
atlas_add_component( TrigOfflineDriftCircleTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TRT_ConditionsServicesLib InDetRawData InDetPrepRawData TrkPrepRawData TrigTimeAlgsLib IRegionSelector InDetIdentifier )

# Install files from the package:
atlas_install_headers( TrigOfflineDriftCircleTool )
atlas_install_python_modules( python/*.py )

