################################################################################
# Package: MuonPRDTest
################################################################################

# Declare the package name:
atlas_subdir( MuonPRDTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonSimData
                          MuonSpectrometer/MuonSimEvent
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonPRDTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier xAODEventInfo GeneratorObjects MuonAGDDDescription MuonReadoutGeometry MuonDigitContainer MuonRDO MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces MuonSimData MuonSimEvent TrkGeometry TrkSurfaces TrkParameters TrkExInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )

