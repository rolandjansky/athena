################################################################################
# Package: TrigConfData
################################################################################

# Declare the package name:
atlas_subdir( TrigConfData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( CxxUtils )

# External dependencies:
find_package( Boost REQUIRED )

# Component(s) in the package:
atlas_add_library ( TrigConfData TrigConfData/*.h src/*.cxx
                    PUBLIC_HEADERS TrigConfData
                    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                    LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_test( ConstIter SOURCES test/itertest.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfData
                POST_EXEC_SCRIPT nopost.sh )
