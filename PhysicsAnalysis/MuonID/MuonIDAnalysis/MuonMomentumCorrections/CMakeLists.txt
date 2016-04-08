################################################################################
# Package: MuonMomentumCorrections
################################################################################

# Declare the package name:
atlas_subdir( MuonMomentumCorrections )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonMomentumCorrectionsLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS MuonMomentumCorrections
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODMuon PATInterfaces MuonSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODBase xAODCore xAODEventInfo GaudiKernel PathResolver )

atlas_add_component( MuonMomentumCorrections
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODMuon PATInterfaces AthenaBaseComps xAODBase xAODCore xAODEventInfo GaudiKernel MuonSelectorToolsLib PathResolver MuonMomentumCorrectionsLib )

atlas_add_dictionary( MuonMomentumCorrectionsDict
                      MuonMomentumCorrections/MuonMomentumCorrectionsDict.h
                      MuonMomentumCorrections/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODMuon PATInterfaces AthenaBaseComps xAODBase xAODCore xAODEventInfo GaudiKernel MuonSelectorToolsLib PathResolver MuonMomentumCorrectionsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

