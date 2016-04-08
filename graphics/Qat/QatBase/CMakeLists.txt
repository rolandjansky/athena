################################################################################
# Package: QatBase
################################################################################

# Declare the package name:
atlas_subdir( QatBase )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Install files from the package:
atlas_install_scripts( share/generate-qat-factory-code share/generate-qat-visfunctions )

