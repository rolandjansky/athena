################################################################################
# Package: TrigCostD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TrigCostD3PDMaker )

# Component(s) in the package:
atlas_add_library( TrigCostD3PDMakerLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel TrigConfHLTData TrigConfL1Data TrigMonitoringEvent D3PDMakerInterfaces TrigConfInterfaces )

atlas_add_component( TrigCostD3PDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel TrigConfHLTData TrigConfL1Data TrigMonitoringEvent TrigCostD3PDMakerLib D3PDMakerInterfaces TrigConfInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

