################################################################################
# Package: CaloSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( CaloSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Calorimeter/CaloCnv/CaloSimEventTPCnv
                          Calorimeter/CaloSimEvent
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_poolcnv_library( CaloSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CaloSimEvent/CaloCalibrationHitContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities CaloSimEventTPCnv CaloSimEvent AthenaPoolCnvSvcLib )

# Install files from the package:
atlas_install_headers( CaloSimEventAthenaPool )

