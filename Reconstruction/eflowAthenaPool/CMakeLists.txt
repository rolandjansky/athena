################################################################################
# Package: eflowAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( eflowAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          Reconstruction/eflowEvent
                          Reconstruction/eflowEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( eflowAthenaPoolPoolCnv
                           src/*.cxx
                           FILES eflowEvent/eflowObjectContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP eflowEvent eflowEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

