################################################################################
# Package: MissingETMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MissingETMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Reconstruction/Jet/JetInterface
                          PRIVATE
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MissingETMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaMonitoringLib JetInterface xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon GaudiKernel )

# Install files from the package:
atlas_install_headers( MissingETMonitoring )
atlas_install_joboptions( share/*.py )

