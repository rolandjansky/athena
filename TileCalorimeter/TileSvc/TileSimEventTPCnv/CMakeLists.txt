################################################################################
# Package: TileSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TileSimEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          TileCalorimeter/TileSimEvent
                          PRIVATE
                          AtlasTest/TestTools
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TileSimEventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS TileSimEventTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} TileSimEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES TestTools GaudiKernel )

atlas_add_dictionary( TileSimEventTPCnvDict
                      TileSimEventTPCnv/TileSimEventTPCnvDict.h
                      TileSimEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileSimEvent TestTools GaudiKernel TileSimEventTPCnv )

atlas_add_dictionary( OLD_TileSimEventTPCnvDict
                      TileSimEventTPCnv/TileSimEventTPCnvDict.h
                      TileSimEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileSimEvent TestTools GaudiKernel TileSimEventTPCnv )

atlas_add_test( TileHitCnv_p1_test
                SOURCES
                test/TileHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileSimEvent TestTools GaudiKernel TileSimEventTPCnv )

atlas_add_test( TileHitVectorCnv_p1_test
                SOURCES
                test/TileHitVectorCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib TileSimEvent TestTools GaudiKernel TileSimEventTPCnv )

