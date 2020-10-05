# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTracking )

# Extra dependencies, based on what environment we are in.
if( NOT XAOD_STANDALONE AND NOT XAOD_ANALYSIS )
   set( extra_libs TrkNeutralParameters TrkParameters TrkTrack VxVertex )
endif()

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTracking
   xAODTracking/*.h xAODTracking/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTracking
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers  AthLinks GeoPrimitives
   EventPrimitives xAODBase xAODCore ${extra_libs} )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTracking/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TrackParticleContainer_v1"
              "xAOD::NeutralParticleContainer_v1" "xAOD::VertexContainer_v1"
              "xAOD::TrackMeasurementValidationContainer_v1"
              "xAOD::TrackStateValidationContainer_v1"
              "xAOD::SCTRawHitValidationContainer_v1" )

atlas_add_dictionary( xAODTrackingDict
   xAODTracking/xAODTrackingDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTracking
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package.
atlas_add_test( xAODTracking_TrackParticle_test
   SOURCES test/xAODTracking_TrackParticle_test.cxx
   LINK_LIBRARIES xAODTracking )

atlas_add_test( xAODTracking_TrackParticlexAODHelpers_test
   SOURCES test/xAODTracking_TrackParticlexAODHelpers_test.cxx
   LINK_LIBRARIES xAODTracking
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( ut_xaodtracking_vertex
   SOURCES test/ut_xaodtracking_vertex.cxx
   LINK_LIBRARIES xAODTracking )
