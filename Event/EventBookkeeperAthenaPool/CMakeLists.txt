################################################################################
# Package: EventBookkeeperAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventBookkeeperMetaData
                          Event/EventBookkeeperTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( EventBookkeeperAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventBookkeeperMetaData/EventBookkeeperCollection.h EventBookkeeperMetaData/EventBookkeeper.h EventBookkeeperMetaData/SkimDecisionCollection.h EventBookkeeperMetaData/SkimDecision.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib EventBookkeeperMetaData EventBookkeeperTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

