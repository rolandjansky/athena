# $Id: CMakeLists.txt 779801 2016-10-22 01:08:16Z ssnyder $
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
   Control/AthLinks
   Control/AthContainers
   GaudiKernel )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( SGComps src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TestTools AthenaBaseComps AthenaKernel
   SGTools StoreGateLib SGtests GaudiKernel AthLinks AthContainers )

atlas_install_python_modules( python/*.py )

# Declare the test(s) of the package:
atlas_add_test( SGFolder_test
   SOURCES test/SGFolder_test.cxx src/SGFolder.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES TestTools GaudiKernel SGTools AthenaKernel AthenaBaseComps AthLinks
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

 atlas_add_test( ProxyProviderSvc_test
   SOURCES test/ProxyProviderSvc_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES TestTools GaudiKernel SGTools AthenaKernel AthenaBaseComps AthLinks
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

 atlas_add_test( AddressRemappingSvc_test
   SOURCES test/AddressRemappingSvc_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES TestTools GaudiKernel SGTools AthenaKernel AthenaBaseComps AthLinks
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
