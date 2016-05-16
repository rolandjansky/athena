################################################################################
# Package: TauAnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( TauAnalysisTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Generators/TruthUtils
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )



# Component(s) in the package:
atlas_add_library( TauAnalysisToolsLib
                     Root/*.cxx
		     PUBLIC_HEADERS TauAnalysisTools
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthLinks AsgTools xAODEgamma xAODJet xAODTau xAODTracking xAODTruth TruthUtils PATCoreLib PATInterfaces ElectronPhotonSelectorToolsLib AthenaBaseComps xAODCore xAODEventInfo GaudiKernel PathResolver )

atlas_add_component( TauAnalysisTools
		     src/*.cxx
                     src/components/*.cxx		     
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthLinks AsgTools xAODEgamma xAODJet xAODTau xAODTracking xAODTruth TruthUtils PATCoreLib PATInterfaces ElectronPhotonSelectorToolsLib AthenaBaseComps xAODCore xAODEventInfo GaudiKernel PathResolver TauAnalysisToolsLib )

# Install files from the package:
atlas_install_headers( TauAnalysisTools )
atlas_install_joboptions( share/*.py )

