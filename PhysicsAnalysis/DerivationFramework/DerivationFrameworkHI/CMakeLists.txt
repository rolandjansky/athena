################################################################################
# Package: DerivationFrameworkHI
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkHI )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/HeavyIonPhys/HIEventUtils
                          InnerDetector/InDetRecToolsInDetTrackSelectionTool
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          GaudiKernel
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation )

# External dependencies:
find_package( ROOT )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkHI
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps HIEventUtilsLib GaudiKernel PATCoreLib ExpressionEvaluationLib DerivationFrameworkInterfacesLib)

# Install files from the package:
atlas_install_headers( DerivationFrameworkHI )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

