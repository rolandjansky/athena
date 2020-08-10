################################################################################
# Package: ISF_SimulationSelectors
################################################################################

# Declare the package name:
atlas_subdir( ISF_SimulationSelectors )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( ISF_SimulationSelectors
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} AtlasHepMCLib GaudiKernel BarcodeServicesLib ISF_Event ISF_InterfacesLib ISF_TrackingInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

