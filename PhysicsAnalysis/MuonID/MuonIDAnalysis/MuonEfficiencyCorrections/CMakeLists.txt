################################################################################
# Package: MuonEfficiencyCorrections
################################################################################

# Declare the package name:
atlas_subdir( MuonEfficiencyCorrections )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PhysicsAnalysis/AnalysisCommon/PileupReweighting
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonEfficiencyCorrectionsLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS MuonEfficiencyCorrections
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools xAODEventInfo xAODMuon PATInterfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaBaseComps xAODTrigger GaudiKernel PathResolver )

atlas_add_component( MuonEfficiencyCorrections
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODMuon PATInterfaces AthContainers AthenaBaseComps xAODTrigger GaudiKernel PathResolver MuonEfficiencyCorrectionsLib )

atlas_add_dictionary( MuonEfficiencyCorrectionsDict
                      MuonEfficiencyCorrections/MuonEfficiencyCorrectionsDict.h
                      MuonEfficiencyCorrections/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODMuon PATInterfaces AthContainers AthenaBaseComps xAODTrigger GaudiKernel PathResolver MuonEfficiencyCorrectionsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

