# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MVAUtils )

# External dependencies:
find_package( ROOT COMPONENTS Tree TMVA XMLIO Core MathCore RIO)

# Component(s) in the package:
atlas_add_library( MVAUtils
                   Root/*.cxx
                   PUBLIC_HEADERS MVAUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES})

atlas_add_dictionary( MVAUtilsDict
                      MVAUtils/MVAUtilsDict.h
                      MVAUtils/selection.xml
                      LINK_LIBRARIES MVAUtils)

atlas_install_python_modules( util/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_executable( convertXmlToRootTree util/convertXmlToRootTree.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} MVAUtils
                      LINK_LIBRARIES ${ROOT_LIBRARIES} MVAUtils CxxUtils)

atlas_add_executable( check_timing_mvautils
                      util/check_timing_mvautils.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} MVAUtils
                      LINK_LIBRARIES ${ROOT_LIBRARIES} MVAUtils )

atlas_install_scripts( util/convertLGBMToRootTree.py
                       util/convertXGBoostToRootTree.py )

atlas_add_test( ut_test_MVAUtils.py
                SCRIPT test/ut_test_MVAUtils.py TestMVAUtilsBasic
                PROPERTIES TIMEOUT 300 )
