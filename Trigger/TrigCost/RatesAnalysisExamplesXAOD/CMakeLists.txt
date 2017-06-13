################################################################################
# Package: RatesAnalysisExamplesXAOD
################################################################################

# Declare the package name:
atlas_subdir( RatesAnalysisExamplesXAOD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODEgamma
                          Trigger/TrigCost/RatesAnalysis )

# Component(s) in the package:
atlas_add_component( RatesAnalysisExamplesXAOD
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel xAODJet RatesAnalysis )

# Install files from the package:
atlas_install_headers( RatesAnalysisExamplesXAOD )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

