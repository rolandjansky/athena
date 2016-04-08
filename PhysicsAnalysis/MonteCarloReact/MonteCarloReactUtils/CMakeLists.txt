################################################################################
# Package: MonteCarloReactUtils
################################################################################

# Declare the package name:
atlas_subdir( MonteCarloReactUtils )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread )

# Component(s) in the package:
atlas_add_library( MonteCarloReactUtils
                   src/*.cxx
                   PUBLIC_HEADERS MonteCarloReactUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

