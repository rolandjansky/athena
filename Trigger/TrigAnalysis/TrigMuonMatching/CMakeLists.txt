################################################################################
# Package: TrigMuonMatching
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonMatching )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODMuon
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfxAOD )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigMuonMatchingLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS TrigMuonMatching
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODMuon TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTrigMuon xAODTrigger GaudiKernel )

atlas_add_component( TrigMuonMatching
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODMuon TrigDecisionToolLib AthenaBaseComps xAODTrigMuon xAODTrigger GaudiKernel TrigMuonMatchingLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

