################################################################################
# Package: TrigEgammaMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigAnalysis/TrigEgammaAnalysisTools
                          Control/AthenaMonitoring )

atlas_add_component( TrigEgammaMonitoring
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES TrigEgammaAnalysisToolsLib GaudiKernel AthenaMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
