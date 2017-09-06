################################################################################
# Package: CoraCoolTest
################################################################################

# Declare the package name:
atlas_subdir( CoraCoolTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/CoraCool )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_executable( CoraCoolExample
                      src/CoraCoolExample.cxx
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CoraCool )


atlas_install_scripts( share/CoraCoolExample_SQLite.sh )
