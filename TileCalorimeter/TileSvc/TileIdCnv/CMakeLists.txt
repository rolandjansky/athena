################################################################################
# Package: TileIdCnv
################################################################################

# Declare the package name:
atlas_subdir( TileIdCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/DetDescrCnvSvc
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/IdDictDetDescr
                          GaudiKernel
                          TileCalorimeter/TileIdentifier )

# Component(s) in the package:
atlas_add_component( TileIdCnv
                     src/Tile*.cxx
                     LINK_LIBRARIES DetDescrCnvSvcLib StoreGateLib SGtests IdDictDetDescr GaudiKernel TileIdentifier )

# Install files from the package:
atlas_install_headers( TileIdCnv )
atlas_install_joboptions( share/*.py )

