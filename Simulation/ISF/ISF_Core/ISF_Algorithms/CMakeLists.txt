################################################################################
# Package: ISF_Algorithms
################################################################################

# Declare the package name:
atlas_subdir( ISF_Algorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArSimEvent
                          TileCalorimeter/TileSimEvent
                          MuonSpectrometer/MuonSimEvent
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tools/PmbCxxUtils
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
#find_package( GMock )

# Component(s) in the package:
atlas_add_component( ISF_Algorithms
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps AtlasDetDescr ISF_Event ISF_Interfaces PmbCxxUtils InDetSimEvent LArSimEvent TileSimEvent MuonSimEvent )

atlas_add_library( ISF_AlgorithmsLib
                   src/*.cxx
                   PUBLIC_HEADERS ISF_Algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps AtlasDetDescr ISF_Event ISF_Interfaces PmbCxxUtils InDetSimEvent LArSimEvent TileSimEvent MuonSimEvent )

atlas_add_test( CollectionMerger_test
                SOURCES test/CollectionMerger_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} #${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES TestTools ISF_AlgorithmsLib ${GTEST_LIBRARIES} #${GMOCK_LIBRARIES}
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_headers( ISF_Algorithms )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
