################################################################################
# Package: TileSimUtils
################################################################################

# Declare the package name:
atlas_subdir( TileSimUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# this line failed automatic conversion in cmt2cmake :
# macro_prepend TileSimUtilsDict_shlibflags " -lTileSimUtils "

# Component(s) in the package:
atlas_add_library( TileSimUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TileSimUtils
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier CaloSimEvent AthenaBaseComps SGTools GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_component( TileSimUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier CaloSimEvent AthenaBaseComps SGTools GaudiKernel AthenaKernel StoreGateLib SGtests TileSimUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

