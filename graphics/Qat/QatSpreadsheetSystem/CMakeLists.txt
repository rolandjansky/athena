################################################################################
# Package: QatSpreadsheetSystem
################################################################################

# Declare the package name:
atlas_subdir( QatSpreadsheetSystem )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatBase
                          graphics/Qat/QatDataAnalysis
                          graphics/Qat/QatSystemInterfaces )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Install files from the package:
atlas_install_headers( QatSpreadsheetSystem )

