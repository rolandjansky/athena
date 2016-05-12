# $Id: CMakeLists.txt 729200 2016-03-11 15:37:14Z krasznaa $
################################################################################
# Package: CLIDComps
################################################################################

# Declare the package name:
atlas_subdir( CLIDComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaKernel
   Control/SGTools
   GaudiKernel )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( CLIDComps
   src/*.h src/*.cxx src/components/*.cxx
   NOCLIDDB
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel SGTools GaudiKernel )

# Test(s) in the package:
atlas_add_test( ClassIDSvc_test
   SOURCES test/ClassIDSvc_test.cxx
   LINK_LIBRARIES TestTools AthenaKernel SGTools GaudiKernel
   EXTRA_PATTERNS "WARNING Could not resolve clid DB|^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/Gaudi_clid.db )
atlas_install_scripts( share/clid test/_clid_unittest.py )
