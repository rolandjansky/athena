################################################################################
# Package: TrigMissingEtEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigMissingEtEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          GaudiKernel
                          Tools/AtlasDoxygen
                          PRIVATE
                          Event/xAOD/xAODTrigMissingET
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigMissingEtEvent
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS TrigMissingEtEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools GaudiKernel TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTrigMissingET )

atlas_add_sercnv_library( TrigMissingEtEventSerCnv
                          FILES TrigMissingEtEvent/TrigMissingET.h TrigMissingEtEvent/TrigMissingETContainer.h TrigMissingEtEvent/TrigMissingEtComponent.h
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel xAODTrigMissingET TrigSerializeCnvSvcLib TrigNavigationLib TrigMissingEtEvent )

atlas_add_sercnv_library( TrigMissingEtEventxAODSerCnv
                          FILES xAODTrigMissingET/TrigMissingETContainer.h xAODTrigMissingET/TrigMissingETAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TrigMissingETContainer xAOD::TrigMissingETAuxContainer
                          CNV_PFX xAOD
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel xAODTrigMissingET TrigSerializeCnvSvcLib TrigNavigationLib TrigMissingEtEvent )

atlas_add_dictionary( TrigMissingEtEventDict
                      TrigMissingEtEvent/TrigMissingEtEventDict.h
                      TrigMissingEtEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel xAODTrigMissingET TrigSerializeCnvSvcLib TrigNavigationLib TrigMissingEtEvent
                      DATA_LINKS TrigMissingET )

