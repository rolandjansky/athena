################################################################################
# Package: ForwardRegionProperties
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegionProperties )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          ForwardDetectors/ForwardSimulation/ForwardRegionGeoModel
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( ForwardRegionProperties
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps ForwardRegionGeoModelLib GaudiKernel )

# Install files from the package:
atlas_install_headers( ForwardRegionProperties )
atlas_install_python_modules( python/*.py )

