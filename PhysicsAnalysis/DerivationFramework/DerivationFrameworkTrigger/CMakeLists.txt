################################################################################
# Package: DerivationFrameworkTrigger
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTrigger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigMonitoring/TrigCostMonitor
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Tools/PathResolver
                          External/AtlasROOT)

# External dependencies:
find_package( ROOT COMPONENTS XMLIO XMLParser )
find_package( xml )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkTriggerLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkTrigger
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES xAODEventInfo TrigCostMonitor TrigAnalysisInterfaces )

atlas_add_component( DerivationFrameworkTrigger
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODEventInfo TrigCostMonitor TrigAnalysisInterfaces GaudiKernel DerivationFrameworkTriggerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

