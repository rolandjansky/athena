################################################################################
# Package: IOVDbTestAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( IOVDbTestAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/DatabaseTest/IOVDbTestConditions
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_poolcnv_library( IOVDbTestAthenaPoolPoolCnv
                           src/*.cxx
                           FILES IOVDbTestConditions/IOVDbTestAmdbCorrection.h IOVDbTestConditions/IOVDbTestMDTEleMap.h IOVDbTestConditions/IOVDbTestMDTEleMapColl.h
                           MULT_CHAN_TYPES IOVDbTestMDTEleMapColl
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP IOVDbTestConditions AthenaPoolCnvSvcLib )

