################################################################################
# Package: VP1UtilsBase
################################################################################
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Declare the package name:
atlas_subdir( VP1UtilsBase )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore )

# Component(s) in the package:
atlas_add_library( VP1UtilsBase src/*.cxx
   PUBLIC_HEADERS VP1UtilsBase
   PRIVATE_LINK_LIBRARIES Qt4::QtCore )
