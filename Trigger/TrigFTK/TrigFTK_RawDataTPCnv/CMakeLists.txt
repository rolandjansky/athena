################################################################################
# Package: TrigFTK_RawDataTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigFTK_RawDataTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigFTK/TrigFTK_RawData
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigFTK_RawDataTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigFTK_RawDataTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigFTK_RawData TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigFTK_RawDataTPCnvDict
                      TrigFTK_RawDataTPCnv/TrigFTK_RawDataTPCnvDict.h
                      TrigFTK_RawDataTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigFTK_RawData TestTools AthenaKernel GaudiKernel TrigFTK_RawDataTPCnv )

atlas_add_test( FTK_RawTrackCnv_p1_test
                SOURCES
                test/FTK_RawTrackCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigFTK_RawData TestTools AthenaKernel GaudiKernel TrigFTK_RawDataTPCnv )

