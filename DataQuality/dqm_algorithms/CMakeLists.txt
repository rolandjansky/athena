################################################################################
# Package: dqm_algorithms
################################################################################

# Declare the package name:
atlas_subdir( dqm_algorithms )

# External dependencies:
find_package( Boost COMPONENTS program_options filesystem thread system )
find_package( ROOT COMPONENTS MathCore Hist Core Tree RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( dqm_algorithms
                   src/*.cxx
                   tools/AlgorithmHelper.cxx
                   tools/SimpleAlgorithmConfig.cxx
                   PUBLIC_HEADERS dqm_algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( dqm_tools
                   tools/DumpConfig.cxx
                   PUBLIC_HEADERS dqm_algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( dqm_summaries
                   summary/*.cxx
                   PUBLIC_HEADERS dqm_algorithms
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( dqm_algorithmsDict
                      dqm_algorithms/dqm_algorithmsDict.h
                      dqm_algorithms/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

atlas_add_dictionary( dqm_toolsDict
                      dqm_algorithms/tools/dqm_toolsDict.h
                      dqm_algorithms/tools/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

atlas_add_executable( dqm_print_algorithms
                      test/print_algorithms.cpp
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

atlas_add_executable( dqm_print_summarymakers
                      test/print_summarymakers.cpp
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} dqm_algorithms dqm_tools dqm_summaries )

# Aliases:
atlas_add_alias( dqm_print_algorithms "dqm_print_algorithms.exe" )
atlas_add_alias( dqm_print_summarymakers "dqm_print_summarymakers.exe" )

