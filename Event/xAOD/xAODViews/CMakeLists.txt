################################################################################
# Package: xAODViews
################################################################################

# Declare the package name:
atlas_subdir( xAODViews )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( xAODViewsDict
                      xAODViews/xAODViewsDict.h
                      xAODViews/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODTracking )

# Install files from the package:
atlas_install_headers( xAODViews )

