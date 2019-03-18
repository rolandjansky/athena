################################################################################
# Package: InDetPrepRawDataFormation
################################################################################

# Declare the package name:
atlas_subdir( InDetPrepRawDataFormation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthViews
                          DetectorDescription/AtlasDetDescr
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/TRT_DriftCircleTool
                          InnerDetector/InDetConditions/SCT_ConditionsData
                        )

# Component(s) in the package:
atlas_add_component( InDetPrepRawDataFormation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests IRegionSelector Identifier GaudiKernel InDetRawData InDetPrepRawData SiClusterizationToolLib TrigSteeringEvent AtlasDetDescr InDetIdentifier InDetReadoutGeometry AthViews)

# Install files from the package:
atlas_install_headers( InDetPrepRawDataFormation )

