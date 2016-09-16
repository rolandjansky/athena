# $Id: CMakeLists.txt 773582 2016-09-16 12:12:10Z smh $
################################################################################
# Package: HLTRates2COOL
################################################################################

# Declare the package name:
atlas_subdir( HLTRates2COOL )

# External dependencies:
find_package( Boost COMPONENTS program_options system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Hist )
find_package( tdaq COMPONENTS cmdline rc_CmdLine rc_ItemCtrl oh ohroot owl omnithread omniORB4 is )
find_package( tdaq-common COMPONENTS ers )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( HLTRates2COOL
   HLTRates2COOL/*.h src/*.cxx
   PUBLIC_HEADERS HLTRates2COOL
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES}
   ${TDAQ_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${TBB_LIBRARIES} ${Boost_LIBRARIES} )

atlas_add_executable( HLTCountsArchiver
   src/test/HLTCountsArchiver.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   ${TDAQ_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES}
   ${TDAQ_LIBRARIES} HLTRates2COOL )

atlas_add_executable( testHLTtoCOOL
   src/test/testHLTtoCOOL.cxx
   LINK_LIBRARIES HLTRates2COOL )

atlas_add_executable( HLTRatesFromCOOL
   src/test/HLTRatesFromCOOL.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} HLTRates2COOL )
