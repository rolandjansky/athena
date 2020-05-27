# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODParticleEvent )

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODParticleEvent
   xAODParticleEvent/*.h xAODParticleEvent/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODParticleEvent
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore
   xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTruth )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODParticleEvent/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::ParticleContainer_v1"
              "xAOD::CompositeParticleContainer_v1" )

atlas_add_dictionary( xAODParticleEventDict
   xAODParticleEvent/xAODParticleEventDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODParticleEvent
   EXTRA_FILES Root/dict/*.cxx )
