################################################################################
# Package: ISF_HepMC_Services
################################################################################

# Declare the package name:
atlas_subdir( ISF_HepMC_Services )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Generators/GeneratorObjects
                          Simulation/Barcode/BarcodeInterfaces
                          Simulation/G4Utilities/G4TruthStrategies
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_HepMC/ISF_HepMC_Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( ISF_HepMC_Services
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps StoreGateLib SGtests AtlasDetDescr GeneratorObjects G4TruthStrategies ISF_Event ISF_Interfaces )

# Install files from the package:
atlas_install_headers( ISF_HepMC_Services )
atlas_install_python_modules( python/*.py )

