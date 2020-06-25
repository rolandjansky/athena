################################################################################
# Package: ISF_SimulationSelectors
################################################################################

# Declare the package name:
atlas_subdir( ISF_SimulationSelectors )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Simulation/Barcode/BarcodeServices
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Tracking/ISF_TrackingInterfaces )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_SimulationSelectors
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} ${HEPPDT_LIBRARIES} GaudiKernel BarcodeServicesLib ISF_Event ISF_Interfaces )

# Install files from the package:
atlas_install_headers( ISF_SimulationSelectors )
atlas_install_python_modules( python/*.py )

