# $Id: CMakeLists.txt 751314 2016-06-01 09:14:59Z krasznaa $
################################################################################
# Package: xAODParticleEvent
################################################################################

# Declare the package name:
atlas_subdir( xAODParticleEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODTruth )

# External dependencies:
find_package( ROOT COMPONENTS Core GenVector )

# Extra dependencies, based on what environment we are in:
if (BUILDVP1LIGHT)
    if( BUILDVP1LIGHT_DIST STREQUAL "ubuntu")
        set( extra_libs GenVector )
    endif()
endif()

# Component(s) in the package:
atlas_add_library( xAODParticleEvent
   xAODParticleEvent/*.h xAODParticleEvent/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODParticleEvent
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore xAODEgamma xAODJet
   xAODMissingET xAODMuon xAODTau xAODTruth ${extra_libs} )

atlas_add_dictionary( xAODParticleEventDict
   xAODParticleEvent/xAODParticleEventDict.h
   xAODParticleEvent/selection.xml
   LINK_LIBRARIES ${extra_libs} xAODParticleEvent
   EXTRA_FILES Root/dict/*.cxx )
