################################################################################
# Package: CoralUtilities
################################################################################

# Declare the package name:
atlas_subdir( CoralUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( nlohmann_json )
find_package( ROOT COMPONENTS Core RIO Tree )

# Component(s) in the package:
atlas_add_library( CoralUtilitiesLib
                   src/*.cxx
                   PUBLIC_HEADERS CoralUtilities
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils z nlohmann_json::nlohmann_json )

