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
                          Trigger/TrigCost/EnhancedBiasWeighter
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Tools/PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

