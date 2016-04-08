################################################################################
# Package: LArSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LArSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          LArCalorimeter/LArCnv/LArSimEventTPCnv
                          LArCalorimeter/LArSimEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( LArSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArSimEvent/LArHitContainer.h LArSimEvent/LArHitFloatContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel LArSimEventTPCnv LArSimEvent )

# Install files from the package:
atlas_install_headers( LArSimEventAthenaPool )
atlas_install_joboptions( share/*.py )

