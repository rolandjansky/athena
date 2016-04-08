################################################################################
# Package: xAODParticleEvent
################################################################################

# Declare the package name:
atlas_subdir( xAODParticleEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
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
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODParticleEvent
                   Root/*.cxx
                   PUBLIC_HEADERS xAODParticleEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTruth
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODParticleEventDict
                      xAODParticleEvent/xAODParticleEventDict.h
                      xAODParticleEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau xAODTruth xAODParticleEvent
                      EXTRA_FILES Root/dict/*.cxx )

