################################################################################
# Package: QatPresenterApp
################################################################################

# Declare the package name:
atlas_subdir( QatPresenterApp )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/Qat/QatPresenter )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_executable( present
                      src/present.cpp
                      INCLUDE_DIRS ${QT4_INCLUDE_DIRS}
                      LINK_LIBRARIES ${QT4_LIBRARIES} GL QatPresenter )

