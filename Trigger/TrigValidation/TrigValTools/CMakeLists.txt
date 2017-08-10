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
atlas_install_python_modules( python/*.py bin/chainDump.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( macros/*.C bin/chainDump.py )
atlas_install_scripts( bin/*.py bin/*.pl bin/*.sh )
atlas_install_generic( html/root2html/*.html
                       DESTINATION share/TrigValTools/root2html
                       EXECUTABLE )

