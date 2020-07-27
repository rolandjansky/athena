# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetAnalysisExample )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree Hist RIO )

# Component(s) in the package:
atlas_add_library( TrigInDetAnalysisExampleLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetAnalysisExample
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib EventInfo GeneratorObjects InDetBeamSpotServiceLib McParticleEvent TrigCompositeUtilsLib TrigDecisionToolLib TrigHLTMonitoringLib TrigInDetAnalysis TrigInDetAnalysisUtils TrigSteeringEvent VxVertex egammaEvent muonEvent tauEvent xAODEventInfo xAODTracking
                   PRIVATE_LINK_LIBRARIES AthenaMonitoringLib TrkParameters TrkTrack xAODTruth )

atlas_add_component( TrigInDetAnalysisExample
                     src/components/*.cxx
                     LINK_LIBRARIES TrigInDetAnalysisExampleLib )

# Disable naming convention checker.
# FIXME: This should be fixed properly once run2 is finished.
if( ${CMAKE_CXX_FLAGS} MATCHES "libchecker_gccplugins" )
  set_target_properties( TrigInDetAnalysisExampleLib PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TrigInDetAnalysisExample PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
endif()
