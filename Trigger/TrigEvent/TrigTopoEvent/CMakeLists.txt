################################################################################
# Package: TrigTopoEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigTopoEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          GaudiKernel
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          PRIVATE
                          Event/EventPrimitives
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigTopoEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigTopoEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools GaudiKernel egammaEvent tauEvent TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives )

atlas_add_sercnv_library( TrigTopoEventSerCnv
                          FILES TrigTopoEvent/ElectronMuonTopoInfo.h TrigTopoEvent/ElectronMuonTopoInfoContainer.h
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel egammaEvent tauEvent TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle EventPrimitives TrigSerializeCnvSvcLib TrigNavigationLib TrigTopoEvent )

atlas_add_dictionary( TrigTopoEventDict
                      TrigTopoEvent/TrigTopoEventDict.h
                      TrigTopoEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel egammaEvent tauEvent TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle EventPrimitives TrigSerializeCnvSvcLib TrigNavigationLib TrigTopoEvent
                      DATA_LINKS ElectronMuonTopoInfo
                      ELEMENT_LINKS ElectronMuonTopoInfoContainer )

