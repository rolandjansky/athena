################################################################################
# Package: QatPlotting
################################################################################

# Declare the package name:
atlas_subdir( QatPlotting )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatDataAnalysis )

# External dependencies:
find_package( CLHEP )
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( QatPlotting
                   src/*.c*
                   PUBLIC_HEADERS QatPlotting
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${QT4_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} QatDataAnalysis ${QT4_LIBRARIES} GL )

