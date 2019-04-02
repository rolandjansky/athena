################################################################################
# Package: ISF_FastCaloSimServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/IdDictParser
                          Control/AthenaBaseComps
                          Simulation/Barcode/BarcodeEvent
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Calorimeter/CaloInterface
                          Calorimeter/CaloEvent
                          Calorimeter/CaloDetDescr
                          Control/StoreGate
                          Event/NavFourMom
                          Generators/GeneratorObjects
                          Simulation/FastShower/FastCaloSim
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimEvent
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimInterfaces
                          Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimParametrization
                          Tools/PathResolver )



# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package(lwtnn)

# Component(s) in the package:
atlas_add_component( ISF_FastCaloSimServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${LWTNN_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel IdDictParser ISF_Interfaces TrkEventPrimitives TrkExInterfaces CaloEvent StoreGateLib SGtests NavFourMom GeneratorObjects FastCaloSimLib ISF_Event ISF_FastCaloSimEvent ISF_FastCaloSimInterfaces ISF_FastCaloSimParametrizationLib PathResolver)

# Install files from the package:
atlas_install_headers( ISF_FastCaloSimServices )
atlas_install_python_modules( python/*.py )

