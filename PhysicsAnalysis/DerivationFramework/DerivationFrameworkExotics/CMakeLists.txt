################################################################################
# Package: DerivationFrameworkExotics
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkExotics )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODJet
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Reconstruction/Jet/JetCalibTools
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkExoticsLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkExotics
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps xAODJet TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODCore xAODEventInfo GaudiKernel )

atlas_add_component( DerivationFrameworkExotics
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps xAODJet TrigDecisionToolLib xAODCore xAODEventInfo GaudiKernel DerivationFrameworkExoticsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

