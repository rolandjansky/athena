################################################################################
# Package: ISF_Algorithms
################################################################################

# Declare the package name:
atlas_subdir( ISF_Algorithms )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( ISF_Algorithms
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel
   AthenaBaseComps StoreGateLib AtlasDetDescr GeneratorObjects HepMC_InterfacesLib
   ISF_Event ISF_InterfacesLib PmbCxxUtils InDetSimEvent LArSimEvent TileSimEvent
   MuonSimEvent TrackRecordLib )

atlas_add_test( CollectionMerger_test
   SOURCES test/CollectionMerger_test.cxx src/CollectionMerger.h
   src/CollectionMerger.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
   LINK_LIBRARIES TestTools GaudiKernel AthenaBaseComps AtlasDetDescr
   ISF_Event ISF_InterfacesLib PmbCxxUtils InDetSimEvent LArSimEvent TileSimEvent
   MuonSimEvent ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test(collection_merger_helpers_tests
               SCRIPT python/test/test_collection_merger_helpers.py)

atlas_add_test( SimKernelMT_test
                SOURCES
                test/SimKernelMT_test.cxx
                src/SimKernelMT.cxx
                INCLUDE_DIRS
                ${EIGEN_INCLUDE_DIRS}
                ${GTEST_INCLUDE_DIRS}
                ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES
                ISF_Event
                ISF_InterfacesLib
                ${EIGEN_LIBRARIES}
                ${GTEST_LIBRARIES}
                ${GMOCK_LIBRARIES}
                AthenaBaseComps
                GeneratorObjects
                HepMC_InterfacesLib
                TrackRecordLib
                ENVIRONMENT
                "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
                )
# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( ISF_Algorithms_SimKernelMT_test PROPERTIES ENABLE_EXPORTS True )

# Install files from the package:
atlas_install_python_modules( python/*.py )
