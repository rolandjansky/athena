################################################################################
# Package: TrigNavTools
################################################################################

# Declare the package name:
atlas_subdir( TrigNavTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigNavigation
                          PRIVATE
                          Control/StoreGate
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrigNavToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigNavTools
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigDecisionToolLib TrigNavigationLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} xAODTrigger GaudiKernel TrigConfHLTData TrigSteeringEvent )

atlas_add_component( TrigNavTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel TrigDecisionToolLib TrigNavigationLib StoreGateLib SGtests xAODTrigger GaudiKernel TrigConfHLTData TrigSteeringEvent TrigNavToolsLib )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/TrigNavToolsConfig.py )
atlas_install_joboptions( share/*.py )

