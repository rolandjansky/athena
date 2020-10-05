################################################################################
# Package: InDetCoolCoralClientUtils
################################################################################

# Declare the package name:
atlas_subdir( InDetCoolCoralClientUtils )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetCoolCoralClientUtils
                   src/*.cpp
                   PUBLIC_HEADERS InDetCoolCoralClientUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils )

