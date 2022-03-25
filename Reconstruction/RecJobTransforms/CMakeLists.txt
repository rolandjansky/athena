# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecJobTransforms )

# Install python modules
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
# Install RDOtoRDOtrigger job opts with flake8 check
atlas_install_joboptions( share/skeleton.RDOtoRDOtrigger*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
# Install other job opts without flake8 check
atlas_install_joboptions( share/*.py EXCLUDE share/*RDOtoRDOtrigger*.py )
# Install scripts
atlas_install_runtime( scripts/*.py )

atlas_install_data( share/*.ref )

atlas_install_scripts( test/*.py )
atlas_install_scripts( test/*.sh )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_RecoRAW )
# This test takes way too long to run in the dbg build.
if ( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  atlas_add_test( RecoRAW
    SCRIPT test_new_jo_raw_reco.sh
    POST_EXEC_SCRIPT nopost.sh
    PROPERTIES TIMEOUT 900
    PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_RecoRAW )
endif()

# file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_RecoESD )
# atlas_add_test( RecoESD
#    SCRIPT python -m RecJobTransforms.RecoSteering --ESD
#    POST_EXEC_SCRIPT nopost.sh
#    PROPERTIES TIMEOUT 300
#    PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_RecoESD )
