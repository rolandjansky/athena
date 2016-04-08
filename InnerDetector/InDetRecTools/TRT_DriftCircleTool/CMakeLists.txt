################################################################################
# Package: TRT_DriftCircleTool
################################################################################

# Declare the package name:
atlas_subdir( TRT_DriftCircleTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkPrepRawData
                          PRIVATE
                          Commission/CommissionEvent
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TRT_DriftCircleTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib InDetRawData InDetPrepRawData TrkPrepRawData CommissionEvent GeoPrimitives EventPrimitives xAODEventInfo InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( TRT_DriftCircleTool )

