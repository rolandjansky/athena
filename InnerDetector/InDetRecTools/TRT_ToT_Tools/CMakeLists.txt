################################################################################
# Package: TRT_ToT_Tools
################################################################################

# Declare the package name:
atlas_subdir( TRT_ToT_Tools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TRT_ToT_Tools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib TrkEventPrimitives TrkParameters TrkTrack StoreGateLib SGtests AthenaPoolUtilities xAODEventInfo InDetIdentifier InDetReadoutGeometry InDetRIO_OnTrack )

# Install files from the package:
atlas_install_headers( TRT_ToT_Tools )

