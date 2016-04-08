################################################################################
# Package: Herwigpp_i
################################################################################

# Declare the package name:
atlas_subdir( Herwigpp_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          PRIVATE
                          Event/EventInfo
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Herwigpp )
find_package( ThePEG )

add_definitions(-DHWVERSION=\"${HERWIGPP_VERSION}\")

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Herwigpp_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HERWIGPP_INCLUDE_DIRS} ${THEPEG_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HERWIGPP_LIBRARIES} ${THEPEG_LIBRARIES} GeneratorModulesLib EventInfo GaudiKernel PathResolver )

# Install files from the package:
atlas_install_headers( Herwigpp_i )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

