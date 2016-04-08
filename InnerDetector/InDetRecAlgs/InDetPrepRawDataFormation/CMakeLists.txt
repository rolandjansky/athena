################################################################################
# Package: InDetPrepRawDataFormation
################################################################################

# Declare the package name:
atlas_subdir( InDetPrepRawDataFormation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          InnerDetector/InDetRecTools/TRT_DriftCircleTool )

# Component(s) in the package:
atlas_add_component( InDetPrepRawDataFormation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel InDetRawData InDetPrepRawData StoreGateLib SGtests AtlasDetDescr InDetIdentifier InDetReadoutGeometry SiClusterizationToolLib )

# Install files from the package:
atlas_install_headers( InDetPrepRawDataFormation )

