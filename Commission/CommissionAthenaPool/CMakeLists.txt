################################################################################
# Package: CommissionAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( CommissionAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Commission/CommissionEvent
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_poolcnv_library( CommissionAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CommissionEvent/ComTime.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel CommissionEvent AthenaPoolCnvSvcLib )

