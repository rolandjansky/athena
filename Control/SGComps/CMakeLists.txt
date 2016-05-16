# $Id: CMakeLists.txt 729207 2016-03-11 15:50:33Z krasznaa $
################################################################################
# Package: SGComps
################################################################################

# Declare the package name:
atlas_subdir( SGComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   GaudiKernel )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( SGComps src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TestTools AthenaBaseComps AthenaKernel
   SGTools StoreGateLib SGtests GaudiKernel )

# Declare the test(s) of the package:
atlas_add_test( SGFolder_test
   SOURCES test/SGFolder_test.cxx src/SGFolder.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES TestTools GaudiKernel SGTools AthenaKernel AthenaBaseComps
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
