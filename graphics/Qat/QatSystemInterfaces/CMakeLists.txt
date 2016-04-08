################################################################################
# Package: QatSystemInterfaces
################################################################################

# Declare the package name:
atlas_subdir( QatSystemInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/Qat/QatPlotWidgets
                          graphics/Qat/QatPlotting )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( QatSystemInterfaces
                   src/*.c*
                   PUBLIC_HEADERS QatSystemInterfaces
                   INCLUDE_DIRS ${QT4_INCLUDE_DIRS}
                   LINK_LIBRARIES ${QT4_LIBRARIES} GL
                   PRIVATE_LINK_LIBRARIES QatPlotWidgets QatPlotting )

