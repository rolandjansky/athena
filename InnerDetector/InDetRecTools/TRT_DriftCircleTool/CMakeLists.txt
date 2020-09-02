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
                          LumiBlock/LumiBlockData
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool )

# External dependencies:
find_package( Eigen )

atlas_add_library( TRT_DriftCircleToolLib
                   TRT_DriftCircleTool/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_DriftCircleTool
                   LINK_LIBRARIES GaudiKernel InDetPrepRawData InDetRawData TrkPrepRawData AthenaBaseComps TRT_ConditionsServicesLib TRT_ReadoutGeometry CommissionEvent StoreGateLib LumiBlockData )

# Component(s) in the package:
atlas_add_component( TRT_DriftCircleTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} TRT_DriftCircleToolLib  GeoPrimitives EventPrimitives xAODEventInfo InDetIdentifier InDetReadoutGeometry  TRT_DriftFunctionToolLib )


