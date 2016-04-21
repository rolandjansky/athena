################################################################################
# Package: JetMonitoring
################################################################################

# Declare the package name:
atlas_subdir( JetMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEvent
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODJet
                          GaudiKernel
                          Reconstruction/Jet/JetInterface
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaMonitoring
                          Reconstruction/Jet/JetUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( JetMonitoring
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODHIEvent xAODJet GaudiKernel JetInterface AthContainers AthenaMonitoringLib JetUtils )

# Install files from the package:
atlas_install_headers( JetMonitoring )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

