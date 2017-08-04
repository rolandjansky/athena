################################################################################
# Package: DerivationFrameworkFlavourTag
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkFlavourTag )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODCaloEvent
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
                          Reconstruction/Jet/JetInterface
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkFlavourTagLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODCore xAODJet xAODPFlow xAODTracking xAODTrigger xAODTruth JetInterface TrkExInterfaces)

atlas_add_component( DerivationFrameworkFlavourTag
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps xAODCore xAODJet xAODPFlow xAODTracking xAODTrigger JetInterface DerivationFrameworkFlavourTagLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

