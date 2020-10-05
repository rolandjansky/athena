# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigValTools )

# External dependencies:
find_package( ROOT COMPONENTS Hist Graf Gpad RIO Core MathCore Postscript )

# Component(s) in the package:
atlas_add_library( TrigValTools
                   src/*.cxx
                   PUBLIC_HEADERS TrigValTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrigValToolsDict
                      TrigValTools/TrigValToolsDict.h
                      TrigValTools/selection.xml
                      LINK_LIBRARIES TrigValTools )

# Install files from the package:
atlas_install_python_modules( python/*.py python/TrigValSteering bin/chainDump.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.py test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.pl bin/*.sh )
atlas_install_data( share/*.json share/*.conf )

# Unit tests:
atlas_add_test( TrigValSteering_unit_test
                SCRIPT test_unit_trigvalsteering.py
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( rootcomp
                SCRIPT test/test_rootcomp.sh ${CMAKE_CURRENT_SOURCE_DIR}/test/test_rootcomp.C
                PROPERTIES TIMEOUT 450
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( chainDump
                SCRIPT test/test_chainDump.sh ${CMAKE_CURRENT_SOURCE_DIR}/test/test_chainDump.C
                LOG_IGNORE_PATTERN "Processing.*test_chainDump" )
