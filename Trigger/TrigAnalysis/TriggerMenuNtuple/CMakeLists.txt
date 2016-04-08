################################################################################
# Package: TriggerMenuNtuple
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenuNtuple )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TriggerMenuNtuple
                   src/*.cxx
                   PUBLIC_HEADERS TriggerMenuNtuple
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TriggerMenuNtupleDict
                      TriggerMenuNtuple/TriggerMenuNtupleDict.h
                      TriggerMenuNtuple/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent TriggerMenuNtuple )

