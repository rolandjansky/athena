################################################################################
# Package: EventTagAthenaPOOL
################################################################################

# Declare the package name:
atlas_subdir( EventTagAthenaPOOL )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PhysicsAnalysis/EventTag/EventTagTPCnv
                          PhysicsAnalysis/EventTag/TagEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( EventTagAthenaPOOLPoolCnv
                           src/*.cxx
                           FILES TagEvent/RawInfoSummaryForTag.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities EventTagTPCnv TagEvent )

