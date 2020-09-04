# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AthenaPoolUtilities
   AthenaPoolUtilities/*.h AthenaPoolUtilities/*.icc
   INTERFACE
   PUBLIC_HEADERS AthenaPoolUtilities
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} AthContainers AthenaKernel CollectionBase CxxUtils DBDataModel GaudiKernel PersistentDataModel )

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
atlas_install_python_modules( python/*.py)

# Install the CMake code from the package:
atlas_install_generic( cmake/AthenaPoolUtilitiesTestConfig.cmake
   DESTINATION cmake )
atlas_install_generic( share/run_tpcnv_legacy_test.sh.in
   DESTINATION cmake/modules/scripts
   EXECUTABLE )
atlas_install_generic( share/run_tpcnv_test.sh.in
   DESTINATION cmake/modules/scripts
   EXECUTABLE )

# Help packages coming after this one in finding the CMake code of the
# package:
set( AthenaPoolUtilitiesTest_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake
   CACHE PATH "Directory holding the AthenaPoolUtilititesTest module" )
