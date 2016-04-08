################################################################################
# Package: TopPhysValMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TopPhysValMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TopPhysValMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODTau GaudiKernel AthenaBaseComps )

# Install files from the package:
atlas_install_headers( TopPhysValMonitoring )

