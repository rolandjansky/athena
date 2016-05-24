################################################################################
# Package: TrigCostD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TrigCostD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigMonitoringEvent )

# Component(s) in the package:
atlas_add_library( TrigCostD3PDMakerLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools GaudiKernel TrigConfHLTData TrigConfL1Data TrigMonitoringEvent )

atlas_add_component( TrigCostD3PDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TrigConfHLTData TrigConfL1Data TrigMonitoringEvent TrigCostD3PDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

