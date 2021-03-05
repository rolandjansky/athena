################################################################################
# Package: ISF_ActsTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_ActsTools )

find_package( Acts CONFIG REQUIRED COMPONENTS Core PluginJson Fatras )

# Component(s) in the package:
atlas_add_component( ISF_ActsTools
                     src/*.cxx
                     src/components/*.cxx
		             INCLUDE_DIRS
                     ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ActsCore ActsFatras AthenaBaseComps
                     TrkTrack ISF_Event AthenaKernel AtlasDetDescr GeoPrimitives
                     GaudiKernel GeneratorObjects ISF_Event StoreGateLib SGtests
                     GeoModelUtilities ActsInteropLib ActsGeometryLib
                     ISF_InterfacesLib ActsPluginJson ISF_FatrasInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

