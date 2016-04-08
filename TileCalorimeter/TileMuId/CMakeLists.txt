################################################################################
# Package: TileMuId
################################################################################

# Declare the package name:
atlas_subdir( TileMuId )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          TileCalorimeter/TileEvent )

# Component(s) in the package:
atlas_add_component( TileMuId
                     src/Tile*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel CaloEvent CaloIdentifier AthenaKernel TileEvent )

# Install files from the package:
atlas_install_headers( TileMuId )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*jobOptions*.py )

