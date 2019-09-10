################################################################################
# Package: AthenaInterprocess
################################################################################

# Declare the package name:
atlas_subdir( AthenaInterprocess )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
   Control/AthenaKernel
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( UUID )

# Component(s) in the package:
atlas_add_library( AthenaInterprocess
   AthenaInterprocess/*.h src/*.cxx
   PUBLIC_HEADERS AthenaInterprocess
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${UUID_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${UUID_LIBRARIES} AthenaKernel GaudiKernel
   PRIVATE_LINK_LIBRARIES ${CMAKE_DL_LIBS} )
