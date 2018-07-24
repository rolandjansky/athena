################################################################################
# Package: TrigBphysicsEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigBphysicsEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          PRIVATE
                          Control/AthContainers
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigBphysicsEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigBphysicsEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} TrigInDetEvent TrigMuonEvent TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers )

atlas_add_sercnv_library( TrigBphysicsEventxAODSerCnv
                          FILES xAODTrigBphys/TrigBphysContainer.h xAODTrigBphys/TrigBphysAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TrigBphysContainer xAOD::TrigBphysAuxContainer
                          CNV_PFX xAOD
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TrigInDetEvent TrigMuonEvent AthContainers TrigSerializeCnvSvcLib TrigNavigationLib TrigBphysicsEvent )

atlas_add_dictionary( TrigBphysicsEventDict
                      TrigBphysicsEvent/TrigBphysicsEventDict.h
                      TrigBphysicsEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TrigInDetEvent TrigMuonEvent AthContainers TrigSerializeCnvSvcLib TrigNavigationLib TrigBphysicsEvent )

