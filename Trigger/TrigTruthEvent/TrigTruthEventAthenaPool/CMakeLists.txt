################################################################################
# Package: TrigTruthEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrigTruthEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigInDetEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/RootConversions
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent
                          Trigger/TrigTruthEvent/TrigTruthEventTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigTruthEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrigInDetTruthEvent/TrigInDetTrackTruthMap.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities TrigInDetEvent AthenaPoolCnvSvcLib RootConversions TrigInDetTruthEvent TrigTruthEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

