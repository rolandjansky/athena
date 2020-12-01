################################################################################
# Package: VrtSecDecay
################################################################################

# Declare the package name:
atlas_subdir( VrtSecDecay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/RecoTools/ITrackToVertex
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Tracking/TrkEvent/TrkTrackSummary )

# Component(s) in the package:
atlas_add_library( VrtSecDecayLib
                   src/*.cxx
                   PUBLIC_HEADERS VrtSecDecay
                   INCLUDE_DIRS
                   LINK_LIBRARIES
                   AthenaBaseComps GaudiKernel StoreGateLib 
                   xAODEventInfo xAODTracking ITrackToVertex TrkVKalVrtFitterLib
                   PRIVATE_LINK_LIBRARIES xAODEventInfo TrkTrackSummary )

atlas_add_component( VrtSecDecay
                     src/components/*.cxx
                     INCLUDE_DIRS
                     LINK_LIBRARIES
                     AthenaBaseComps GaudiKernel StoreGateLib 
                     xAODEventInfo xAODTracking ITrackToVertex TrkVKalVrtFitterLib
                     TrkTrackSummary VrtSecDecayLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
