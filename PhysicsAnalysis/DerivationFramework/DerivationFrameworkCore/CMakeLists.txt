################################################################################
# Package: DerivationFrameworkCore
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/AthLinks
			  Event/xAOD/xAODBase
                          Event/xAOD/xAODEgamma
			  Event/xAOD/xAODMuon
			  Trigger/TrigAnalysis/TriggerMatchingTool
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkCoreLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkCore
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthContainers AthLinks SGTools  xAODEgamma xAODBase xAODMuon TriggerMatchingToolLib AthenaKernel SGTools AsgAnalysisInterfaces AthAnalysisBaseCompsLib GoodRunsListsLib PathResolver )

atlas_add_component( DerivationFrameworkCore
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthContainers AthLinks xAODBase xAODEgamma xAODMuon TriggerMatchingToolLib AthenaBaseComps GaudiKernel AthenaKernel SGTools StoreGateLib SGtests DerivationFrameworkCoreLib AsgAnalysisInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/frozen_derivation_test.py )
