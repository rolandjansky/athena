################################################################################
# Package: TrigCostPython
################################################################################

# Declare the package name:
atlas_subdir( TrigCostPython )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_xmls( data/*.xml )
atlas_install_xmls( data/*.xml )
atlas_install_generic( data/*.html data/*.js
                       DESTINATION share/TrigCostPython
                       EXECUTABLE )
atlas_install_generic( macros/*.py macros/getfile.sh
                       DESTINATION share/bin
                       EXECUTABLE )

