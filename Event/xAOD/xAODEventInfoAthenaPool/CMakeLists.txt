################################################################################
# Package: xAODEventInfoAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODEventInfoAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventInfoAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEventInfo/EventInfo.h xAODEventInfo/EventAuxInfo.h xAODEventInfo/EventInfoContainer.h xAODEventInfo/EventInfoAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::EventInfo xAOD::EventAuxInfo xAOD::EventInfoContainer xAOD::EventInfoAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEventInfo )

