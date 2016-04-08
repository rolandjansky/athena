################################################################################
# Package: egammaLayerRecalibTool
################################################################################

# Declare the package name:
atlas_subdir( egammaLayerRecalibTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread )

# Component(s) in the package:
atlas_add_library( egammaLayerRecalibTool
                   Root/*.cxx
                   PUBLIC_HEADERS egammaLayerRecalibTool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES PathResolver )

# Install files from the package:
atlas_install_runtime( share/*.root )

