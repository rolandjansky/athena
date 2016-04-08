################################################################################
# Package: TileIdentifier
################################################################################

# Declare the package name:
atlas_subdir( TileIdentifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          DetectorDescription/Identifier
                          PRIVATE
                          DetectorDescription/IdDict
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TileIdentifier
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDict GaudiKernel )

atlas_add_dictionary( TileIdentifierDict
                      TileIdentifier/TileIdentifierDict.h
                      TileIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier Identifier IdDict GaudiKernel TileIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py )

