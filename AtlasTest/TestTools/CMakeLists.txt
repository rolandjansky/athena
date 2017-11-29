# $Id: CMakeLists.txt 744649 2016-05-03 19:33:39Z krasznaa $
################################################################################
# Package: TestTools
################################################################################

# Declare the package name:
atlas_subdir( TestTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaCommon
   TestPolicy
   PRIVATE
   GaudiKernel )

# In standalone mode we just use the headers from the package. While in
# offline mode we build a proper library.
if( XAOD_STANDALONE )
   atlas_add_library( TestTools
      TestTools/*.h
      INTERFACE
      PUBLIC_HEADERS TestTools )
else()
   atlas_add_library( TestTools
      TestTools/*.h src/*.cxx
      PUBLIC_HEADERS TestTools
      PRIVATE_LINK_LIBRARIES GaudiKernel )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/runUnitTests.sh share/post.sh scripts/nightlies/TestHelloWorld.sh share/skip_if_libraryMissing.sh )

if( NOT XAOD_STANDALONE )

   find_package( TBB )

   atlas_add_test( ParallelCallTest_test
      INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
      SOURCES test/test_ParallelCallTestExample.cxx
      LINK_LIBRARIES ${TBB_LIBRARIES}  TestTools  )

endif()
