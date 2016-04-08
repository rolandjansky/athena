################################################################################
# Package: TrigT1RPCsteering
################################################################################

# Declare the package name:
atlas_subdir( TrigT1RPCsteering )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Trigger/TrigT1/TrigT1RPClogic
                          PRIVATE
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1RPChardware )

# External dependencies:
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( TrigT1RPCsteering
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel RPCcablingInterfaceLib MuonReadoutGeometry TrigT1RPClogicLib GeneratorObjects MuonDigitContainer MuonIdHelpersLib TrigT1Interfaces TrigT1RPChardwareLib RPChardware )

# Install files from the package:
atlas_install_headers( TrigT1RPCsteering )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

