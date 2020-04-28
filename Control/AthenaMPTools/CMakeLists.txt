################################################################################
# Package: AthenaMPTools
################################################################################

# Declare the package name:
atlas_subdir( AthenaMPTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaInterprocess
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( yampl )

atlas_add_library( AthenaMPToolsLib
                   AthenaMPTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS AthenaMPTools
                   LINK_LIBRARIES AthenaInterprocess GaudiKernel )

# Component(s) in the package:
atlas_add_component( AthenaMPTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${YAMPL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${YAMPL_LIBRARIES} AthenaMPToolsLib AthenaInterprocess GaudiKernel AthenaBaseComps AthenaKernel rt pthread )

