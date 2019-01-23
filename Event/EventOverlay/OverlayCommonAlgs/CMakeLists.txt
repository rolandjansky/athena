################################################################################
# Package: OverlayCommonAlgs
################################################################################

# Declare the package name:
atlas_subdir( OverlayCommonAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/ByteStreamCnvSvc
                          Event/EventInfo
                          Event/EventOverlay/OverlayAlgBase
                          Event/xAOD/xAODJet
                          Generators/GeneratorObjects
                          Reconstruction/RecEvent
                          Simulation/G4Sim/TrackRecord
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( OverlayCommonAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ByteStreamCnvSvcLib StoreGateLib OverlayAlgBase CaloSimEvent EventInfo GeneratorObjects RecEvent xAODJet
                     TrigSteeringEvent TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/*.py )
