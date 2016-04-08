################################################################################
# Package: MinBiasD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( MinBiasD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          PRIVATE
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_library( MinBiasD3PDMakerLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES TrigCaloEvent TrigInDetEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaKernel SGTools GaudiKernel InDetReadoutGeometry InDetPrepRawData D3PDMakerUtils TrkTrack TrkToolInterfaces )

atlas_add_component( MinBiasD3PDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES TrigCaloEvent TrigInDetEvent AthenaKernel SGTools StoreGateLib SGtests GaudiKernel InDetReadoutGeometry InDetPrepRawData D3PDMakerUtils TrkTrack TrkToolInterfaces MinBiasD3PDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

