################################################################################
# Package: JetMonitoring
################################################################################

# Declare the package name:
atlas_subdir( JetMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODJet 
                          GaudiKernel
                          Reconstruction/Jet/JetInterface 
                          Trigger/TrigEvent/TrigDecisionInterface 
                          Control/AthenaMonitoring
                          PRIVATE
                          Control/AthContainers
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Reconstruction/Jet/JetUtils
			  Event/FourMomUtils
                          )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( JetMonitoringLib
   JetMonitoring/*.h
   INTERFACE
   PUBLIC_HEADERS JetMonitoring
   LINK_LIBRARIES xAODJet GaudiKernel AthenaMonitoringLib )


# Component(s) in the package:
atlas_add_component( JetMonitoring
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODHIEvent xAODJet GaudiKernel JetInterface AthContainers AthenaMonitoringLib JetUtils FourMomUtils)

# Install files from the package:
atlas_install_headers( JetMonitoring )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

