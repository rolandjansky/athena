# $Id: CMakeLists.txt 744649 2016-05-03 19:33:39Z krasznaa $
################################################################################
# Package: GoogleTestTools
################################################################################

# Declare the package name:
atlas_subdir( GoogleTestTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthenaCommon
  TestPolicy
  PRIVATE
  GaudiKernel )

find_package( GTest )
find_package( GMock )

# In standalone mode we just use the headers from the package. While in
# offline mode we build a proper library.
if( XAOD_STANDALONE )
  atlas_add_library( GoogleTestTools
    GoogleTestTools/*.h
    INTERFACE
    PUBLIC_HEADERS GoogleTestTools )
else()
  atlas_add_library( GoogleTestTools
    GoogleTestTools/*.h src/*.cxx
    PUBLIC_HEADERS GoogleTestTools
    INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
    LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
    PRIVATE_LINK_LIBRARIES GaudiKernel )
endif()

# Unit tests for InitGaudiGoogleTest:
atlas_add_test( GoogleTestToolsTests
  SOURCES test/gt_GoogleTestTools.cxx
  INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} 
  LINK_LIBRARIES ${GTEST_LIBRARIES} GaudiKernel GoogleTestTools )

# Install files from the package:
# atlas_install_python_modules( python/*.py )
# atlas_install_joboptions( share/*.py )
# atlas_install_scripts( share/runUnitTests.sh share/post.sh )
