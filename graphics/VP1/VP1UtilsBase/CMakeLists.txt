################################################################################
# Package: VP1UtilsBase
################################################################################
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Declare the package name:
atlas_subdir( VP1UtilsBase )

# External dependencies:
find_package( Qt5 COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( VP1UtilsBase
   VP1UtilsBase/*.h src/*.cxx
   PUBLIC_HEADERS VP1UtilsBase
   PRIVATE_LINK_LIBRARIES Qt5::Core )

# Install files from the package:
atlas_install_runtime( share/*.vp1 ) # default VP1 config files for batch mode
atlas_install_runtime( share/*.png ) # ATLAS logo
