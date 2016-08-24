# $Id: CMakeLists.txt 769588 2016-08-23 13:39:00Z krasznaa $
################################################################################
# Package: InDetTrackSelectionTool
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackSelectionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTracking
   GaudiKernel
   PhysicsAnalysis/AnalysisCommon/PATCore
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkTools/TrkToolInterfaces
   PRIVATE
   Control/AthenaBaseComps
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/VxVertex )

# Component(s) in the package:
atlas_add_library( InDetTrackSelectionToolLib
   InDetTrackSelectionTool/*.h
   INTERFACE
   PUBLIC_HEADERS InDetTrackSelectionTool
   LINK_LIBRARIES PATCoreLib xAODTracking TrkParameters )

atlas_add_component( InDetTrackSelectionTool
   src/*.cxx Root/*.cxx src/components/*.cxx
   LINK_LIBRARIES AsgTools GaudiKernel InDetRIO_OnTrack TrkEventPrimitives
   TrkRIO_OnTrack TrkTrack TrkTrackSummary VxVertex TrkExInterfaces
   TrkToolInterfaces AthenaBaseComps InDetTrackSelectionToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
