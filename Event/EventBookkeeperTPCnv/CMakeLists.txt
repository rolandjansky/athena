################################################################################
# Package: EventBookkeeperTPCnv
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventBookkeeperMetaData
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( EventBookkeeperTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventBookkeeperTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventBookkeeperMetaData TestTools AthenaKernel CxxUtils )

atlas_add_dictionary( EventBookkeeperTPCnvDict
                      EventBookkeeperTPCnv/EventBookkeeperTPCnvDict.h
                      EventBookkeeperTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventBookkeeperMetaData TestTools AthenaKernel CxxUtils EventBookkeeperTPCnv )

atlas_add_test( EventBookkeeperCnv_p1_test
                SOURCES
                test/EventBookkeeperCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventBookkeeperMetaData TestTools AthenaKernel CxxUtils EventBookkeeperTPCnv )

atlas_add_test( EventBookkeeperCnv_p2_test
                SOURCES
                test/EventBookkeeperCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventBookkeeperMetaData TestTools AthenaKernel CxxUtils EventBookkeeperTPCnv )

atlas_add_test( SkimDecisionCnv_p1_test
                SOURCES
                test/SkimDecisionCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventBookkeeperMetaData TestTools AthenaKernel CxxUtils EventBookkeeperTPCnv )

