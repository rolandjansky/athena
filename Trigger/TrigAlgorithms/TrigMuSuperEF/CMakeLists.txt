################################################################################
# Package: TrigMuSuperEF
################################################################################

# Declare the package name:
atlas_subdir( TrigMuSuperEF )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Control/CxxUtils
                          Control/AthenaKernel
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
			  Event/FourMomUtils
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
			  Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigMuonToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

include_directories(src)

# Component(s) in the package:
atlas_add_component( TrigMuSuperEF
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CxxUtils xAODMuon xAODTracking FourMomUtils GaudiKernel MuonPattern MuonSegment MuonRecToolInterfaces MuidEvent MuonCombinedEvent MuonCombinedToolInterfaces Particle TrkSegment TrkToolInterfaces TrigConfHLTData TrigMuonEvent TrigNavigationLib TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/test_TrigMuSuperEF.py share/TrigMuSuperEF_debug_postOptions.py )

