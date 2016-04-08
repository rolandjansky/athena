################################################################################
# Package: VP1UtilitySystems
################################################################################

# Declare the package name:
atlas_subdir( VP1UtilitySystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          graphics/Qat/QatDataAnalysis
                          graphics/Qat/QatPlotWidgets
                          graphics/Qat/QatPlotting )

# External dependencies:
find_package( Coin3D )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1UtilitySystems
                   src/*.c*
                   PUBLIC_HEADERS VP1UtilitySystems
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
                   LINK_LIBRARIES VP1Base GL
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${COIN3D_LIBRARIES} QatDataAnalysis QatPlotWidgets QatPlotting )

