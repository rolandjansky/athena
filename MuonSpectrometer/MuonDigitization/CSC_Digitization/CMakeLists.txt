################################################################################
# Package: CSC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( CSC_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement
                          PRIVATE
                          Control/StoreGate
                          Generators/GeneratorObjects )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( CSC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib xAODEventInfo GaudiKernel MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData MuonSimEvent HitManagement StoreGateLib SGtests GeneratorObjects )

# Install files from the package:
atlas_install_headers( CSC_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

