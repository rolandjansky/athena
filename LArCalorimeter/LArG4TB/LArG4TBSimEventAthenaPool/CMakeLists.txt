################################################################################
# Package: LArG4TBSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LArG4TBSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LArCalorimeter/LArG4TB/LArG4TBSimEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( LArG4TBSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArG4TBSimEvent/LArG4H6FrontHitCollection.h LArG4TBSimEvent/LArG4H6LeakHitCollection.h LArG4TBSimEvent/LArG4H6WarmTCHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArG4TBSimEvent )

