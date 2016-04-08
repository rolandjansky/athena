################################################################################
# Package: InDetTrackSelectionTool
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackSelectionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetTrackSelectionTool
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODTracking GaudiKernel InDetRIO_OnTrack PATCoreLib TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary VxVertex TrkExInterfaces TrkToolInterfaces AthenaBaseComps )

# Install files from the package:
atlas_install_headers( InDetTrackSelectionTool )
atlas_install_joboptions( share/*.py )

