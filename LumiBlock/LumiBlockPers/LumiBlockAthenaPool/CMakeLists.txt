################################################################################
# Package: LumiBlockAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LumiBlockAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LumiBlock/LumiBlockData
                          LumiBlock/LumiBlockPers/LumiBlockTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( LumiBlockAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LumiBlockData/LumiBlockCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LumiBlockData LumiBlockTPCnv )

