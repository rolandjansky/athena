################################################################################
# Package: QatAnnotationSystem
################################################################################

# Declare the package name:
atlas_subdir( QatAnnotationSystem )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatBase
                          graphics/Qat/QatPlotWidgets
                          graphics/Qat/QatSystemInterfaces )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Install files from the package:
atlas_install_headers( QatAnnotationSystem )

