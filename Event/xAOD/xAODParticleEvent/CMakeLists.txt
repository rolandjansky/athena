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

# Component(s) in the package:
atlas_add_library( xAODParticleEvent
   xAODParticleEvent/*.h xAODParticleEvent/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODParticleEvent
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore xAODEgamma xAODJet
   xAODMissingET xAODMuon xAODTau xAODTruth )

atlas_add_dictionary( xAODParticleEventDict
   xAODParticleEvent/xAODParticleEventDict.h
   xAODParticleEvent/selection.xml
   LINK_LIBRARIES xAODParticleEvent
   EXTRA_FILES Root/dict/*.cxx )
