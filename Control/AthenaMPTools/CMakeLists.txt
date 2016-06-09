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

# Component(s) in the package:
atlas_add_component( AthenaMPTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${YAMPL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${YAMPL_LIBRARIES} AthenaInterprocess GaudiKernel AthenaBaseComps AthenaKernel rt pthread )

# Install files from the package:
atlas_install_headers( AthenaMPTools )

