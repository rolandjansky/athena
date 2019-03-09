# $Id: CMakeLists.txt 758198 2016-06-28 15:18:49Z mnowak $
################################################################################
# Package: AthenaPoolUtilities
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaKernel
   Control/AthContainers
   Control/CxxUtils
   Database/AthenaPOOL/DBDataModel
   Database/PersistentDataModel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AthenaPoolUtilities
   AthenaPoolUtilities/*.h AthenaPoolUtilities/*.icc
   INTERFACE
   PUBLIC_HEADERS AthenaPoolUtilities
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel AthContainers DBDataModel
   PersistentDataModel )

atlas_add_dictionary( AthenaPoolUtilitiesCoralDict
   AthenaPoolUtilities/AthenaPoolUtilitiesCoralDict.h
   AthenaPoolUtilities/selectionCoral.xml
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} )

atlas_add_dictionary( AthenaPoolUtilitiesDict
   AthenaPoolUtilities/AthenaPoolUtilitiesDict.h
   AthenaPoolUtilities/selection.xml
   LINK_LIBRARIES AthenaPoolUtilities )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/post_tpcnvtest.sh )

# Install the CMake code from the package:
atlas_install_generic( cmake/AthenaPoolUtilitiesTestConfig.cmake
   DESTINATION cmake )
atlas_install_generic( share/run_tpcnv_test.sh.in
   DESTINATION cmake/modules/scripts
   EXECUTABLE )

# Help packages coming after this one in finding the CMake code of the
# package:
set( AthenaPoolUtilitiesTest_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake
   CACHE PATH "Directory holding the AthenaPoolUtilititesTest module" )
