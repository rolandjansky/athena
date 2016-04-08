################################################################################
# Package: xAODForwardCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODForwardCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODForward
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel )

# Component(s) in the package:
atlas_add_component( xAODForwardCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODForward GaudiKernel TileEvent TileIdentifier AthenaBaseComps AthenaKernel )

# Install files from the package:
atlas_install_headers( xAODForwardCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

