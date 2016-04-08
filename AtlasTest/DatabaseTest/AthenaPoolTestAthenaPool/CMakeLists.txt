################################################################################
# Package: AthenaPoolTestAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolTestAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          PRIVATE
                          AtlasTest/DatabaseTest/AthenaPoolTestData
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( AthenaPoolTestAthenaPoolPoolCnv
                           src/*.cxx
                           FILES AthenaPoolTestData/IAthenaPoolTestCollection.h AthenaPoolTestData/AthenaPoolTestMatrix.h AthenaPoolTestData/AthenaPoolTestMap.h AthenaPoolTestData/FauxTriggerMap.h AthenaPoolTestData/dummy_A.h AthenaPoolTestData/dummy_E.h AthenaPoolTestData/TrigPath.h AthenaPoolTestData/TestGaudiProperty.h AthenaPoolTestData/PixelRDOElemLinkVec.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolTestData AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP )

atlas_add_dictionary( AthenaPoolTestAthenaPoolCnvDict
                      AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolDict.h
                      AthenaPoolTestAthenaPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelAthenaPoolLib AthenaPoolTestData AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP )

# Install files from the package:
atlas_install_headers( AthenaPoolTestAthenaPool )

