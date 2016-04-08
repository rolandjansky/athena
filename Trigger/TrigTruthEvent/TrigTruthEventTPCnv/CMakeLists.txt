################################################################################
# Package: TrigTruthEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigTruthEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RootConversions
                          Generators/GeneratorObjectsTPCnv
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigInDetEventTPCnv
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGTools
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigTruthEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigTruthEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions GeneratorObjectsTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel )

atlas_add_dictionary( TrigTruthEventTPCnvDict
                      TrigTruthEventTPCnv/TrigTruthEventTPCnvDict.h
                      TrigTruthEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions GeneratorObjectsTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigTruthEventTPCnv )

atlas_add_dictionary( OLD_TrigTruthEventTPCnvDict
                      TrigTruthEventTPCnv/TrigTruthEventTPCnvDict.h
                      TrigTruthEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions GeneratorObjectsTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigTruthEventTPCnv )

atlas_add_test( TrigIDHitStatsCnv_p1_test
                SOURCES
                test/TrigIDHitStatsCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions GeneratorObjectsTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigTruthEventTPCnv )

atlas_add_test( TrigInDetTrackTruthCnv_p1_test
                SOURCES
                test/TrigInDetTrackTruthCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions GeneratorObjectsTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigTruthEventTPCnv )

atlas_add_test( TrigInDetTrackTruthMapCnv_p1_test
                SOURCES
                test/TrigInDetTrackTruthMapCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities RootConversions GeneratorObjectsTPCnv TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent TestTools AthenaKernel CxxUtils SGTools GaudiKernel TrigTruthEventTPCnv )

