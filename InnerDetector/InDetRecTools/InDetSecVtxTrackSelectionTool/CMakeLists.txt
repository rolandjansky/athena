################################################################################
# Package: InDetSecVtxTrackSelectionTool
################################################################################

# Declare the package name:
atlas_subdir( InDetSecVtxTrackSelectionTool )

# Declare the package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODTracking
      PhysicsAnalysis/AnalysisCommon/PATCore
      PRIVATE
      Control/xAODRootAccess )
elseif( XAOD_ANALYSIS )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODTracking
      PhysicsAnalysis/AnalysisCommon/PATCore
      GaudiKernel
      PRIVATE
      Control/AthenaBaseComps )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODTracking
      GaudiKernel
      PhysicsAnalysis/AnalysisCommon/PATCore
      Tracking/TrkEvent/TrkParameters
      Tracking/TrkTools/TrkToolInterfaces
      PRIVATE
      Control/AthenaBaseComps
      InnerDetector/InDetRecTools/InDetTrackSelectionTool
      Tracking/TrkEvent/TrkEventPrimitives
      Tracking/TrkEvent/TrkTrack
      Tracking/TrkEvent/TrkTrackSummary
      Tracking/TrkEvent/VxVertex )
endif()

# External(s):
find_package( ROOT COMPONENTS Core RIO Hist MathCore )

# Libraries in the package:
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs TrkParameters GaudiKernel TrkToolInterfaces
      PRIVATE_LINK_LIBRARIES TrkTrack TrkEventPrimitives TrkTrackSummary VxVertex )
endif()

atlas_add_library( InDetSecVtxTrackSelectionToolLib
   InDetSecVtxTrackSelectionTool/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS InDetSecVtxTrackSelectionTool
   LINK_LIBRARIES PATCoreLib xAODTracking ${extra_libs} )

if( NOT XAOD_STANDALONE )
   set( extra_libs )
   if( NOT XAOD_ANALYSIS )
      set( extra_libs TrkTrack TrkTrackSummary )
   endif()
   atlas_add_component( InDetSecVtxTrackSelectionTool
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODTracking
      InDetSecVtxTrackSelectionToolLib ${extra_libs} )
endif()

atlas_add_dictionary( InDetSecVtxTrackSelectionToolDict
   InDetSecVtxTrackSelectionTool/InDetSecVtxTrackSelectionToolDict.h
   InDetSecVtxTrackSelectionTool/selection.xml
   LINK_LIBRARIES InDetSecVtxTrackSelectionToolLib )


# Install files from the package:
atlas_install_joboptions( share/*.py )
