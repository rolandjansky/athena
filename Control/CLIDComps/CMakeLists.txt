# $Id: CMakeLists.txt 787831 2016-12-02 10:18:34Z krasznaa $
################################################################################
# Package: CLIDComps
################################################################################

# Declare the package name:
atlas_subdir( CLIDComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthenaKernel
   Control/SGTools
   AtlasTest/TestTools
   GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS program_options )

# Component(s) in the package:
atlas_add_component( CLIDComps
   src/*.h src/*.cxx src/components/*.cxx
   NOCLIDDB
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel SGTools GaudiKernel )

# Executable(s) in the package:
atlas_add_executable( genCLIDDB
   util/genCLIDDB.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel AthenaKernel )
if( TARGET genCLIDDB AND TARGET CLIDComps )
   add_dependencies( genCLIDDB CLIDCompsComponentsList )
endif()

# Test(s) in the package:
atlas_add_test( ClassIDSvc_test
   SOURCES test/ClassIDSvc_test.cxx
   LINK_LIBRARIES AthenaKernel SGTools GaudiKernel TestTools
   EXTRA_PATTERNS "WARNING Could not resolve clid DB|^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/Gaudi_clid.db )
atlas_install_scripts( share/clid test/_clid_unittest.py )
atlas_install_joboptions( share/*.opts )

# Wait with the build of genCLIDDB until the jobOptions are installed:
if( TARGET genCLIDDB AND TARGET CLIDCompsJobOptInstall )
   add_dependencies( genCLIDDB CLIDCompsJobOptInstall )
endif()
