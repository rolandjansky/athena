################################################################################
# Package: TrigConfData
################################################################################

# Declare the package name:
atlas_subdir( TrigConfData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( )

# External dependencies:
find_package( Boost REQUIRED )

# Component(s) in the package:
atlas_add_library ( TrigConfData TrigConfData/*.h src/*.cxx
                    PUBLIC_HEADERS TrigConfData
                    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                    LINK_LIBRARIES ${Boost_LIBRARIES} )
