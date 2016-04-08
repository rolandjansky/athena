################################################################################
# Package: LumiBlockTPCnv
################################################################################

# Declare the package name:
atlas_subdir( LumiBlockTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LumiBlock/LumiBlockData
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( LumiBlockTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LumiBlockTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LumiBlockData TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( LumiBlockTPCnvDict
                      LumiBlockTPCnv/LumiBlockTPCnvDict.h
                      LumiBlockTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LumiBlockData TestTools AthenaKernel GaudiKernel LumiBlockTPCnv )

atlas_add_test( LumiBlockRangeCnv_p1_test
                SOURCES
                test/LumiBlockRangeCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LumiBlockData TestTools AthenaKernel GaudiKernel LumiBlockTPCnv )

atlas_add_test( LumiBlockRangeCnv_p2_test
                SOURCES
                test/LumiBlockRangeCnv_p2_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib LumiBlockData TestTools AthenaKernel GaudiKernel LumiBlockTPCnv )

