################################################################################
# Package: DerivationFrameworkHiggs
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkHiggs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkHiggsLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkHiggs
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps xAODEgamma xAODJet xAODMuon TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODEventInfo xAODTracking GaudiKernel )

atlas_add_component( DerivationFrameworkHiggs
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps xAODEgamma xAODJet xAODMuon TrigDecisionToolLib xAODEventInfo xAODTracking GaudiKernel DerivationFrameworkHiggsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

