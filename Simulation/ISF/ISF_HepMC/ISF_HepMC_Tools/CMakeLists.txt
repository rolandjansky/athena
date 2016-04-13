################################################################################
# Package: ISF_HepMC_Tools
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Tools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventInfo
                          Generators/GeneratorObjects
                          Generators/TruthUtils
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/Barcode/BarcodeServices
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_HepMC/ISF_HepMC_Interfaces
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag rootGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( ISF_HepMC_Tools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel EventInfo GeneratorObjects TruthUtils BarcodeServicesLib ISF_Event ISF_Interfaces AtlasCLHEP_RandomGenerators PathResolver )

# Install files from the package:
atlas_install_headers( ISF_HepMC_Tools )
atlas_install_python_modules( python/*.py )

