################################################################################
# Package: TrigValTools
################################################################################

# Declare the package name:
atlas_subdir( TrigValTools )

# External dependencies:
find_package( ROOT COMPONENTS Hist Graf Gpad RIO Core Tree MathCore pthread Graf3d Html Postscript Gui GX11TTF GX11 )

include_directories(src)

# Component(s) in the package:
atlas_add_library( TrigValTools
                   src/*.cxx
                   PUBLIC_HEADERS TrigValTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrigValToolsDict
                      TrigValTools/TrigValToolsDict.h
                      TrigValTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigValTools )

# Install files from the package:
atlas_install_python_modules( python/*.py python/TrigValSteering bin/chainDump.py )
atlas_install_data( share/*.json share/*.conf )
atlas_install_runtime( macros/*.C bin/chainDump.py )
atlas_install_scripts( bin/*.py bin/*.pl bin/*.sh test/*.py )
atlas_install_generic( html/root2html/*.html
                       DESTINATION share/TrigValTools/root2html
                       EXECUTABLE )

# Unit tests:
atlas_add_test( TrigValSteering_flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python/TrigValSteering
                POST_EXEC_SCRIPT nopost.sh )

# To be uncommented when ATEAM-562 is fixed
# atlas_add_test( TrigValSteering_unit_test
#                 SCRIPT test_unit_trigvalsteering.py
#                 POST_EXEC_SCRIPT nopost.sh )