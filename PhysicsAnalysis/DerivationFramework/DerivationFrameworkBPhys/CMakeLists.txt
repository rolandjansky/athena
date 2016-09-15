################################################################################
# Package: DerivationFrameworkBPhys
################################################################################
use InDetBeamSpotService          InDetBeamSpotService-*        InnerDetector/InDetConditions

# Declare the package name:
atlas_subdir( DerivationFrameworkBPhys )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODMuon			
                          Control/DataModel
                          Control/AthenaBaseComps
                          External/AtlasBoost
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/JpsiUpsilonTools
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODBPhys
                          Database/IOVDbMetaDataTools
                          Control/AthenaKernel
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Event/EventPrimitives
                          PRIVATE
                          External/AtlasROOT
                          External/GaudiInterface
                          Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
                          Event/xAOD/xAODEventInfo
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODPrimitives
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Reconstruction/RecoTools/TrackToCalo
                          Tracking/TrkEvent/TrkCaloExtension
                          InnerDetector/InDetConditions/InDetBeamSpotService)

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkBPhysLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkHiggs
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES xAODMuon DataModel AthenaBaseComps AtlasBoost DerivationFrameworkInterfaces JpsiUpsilonTools xAODTracking xAODBPhys IOVDbMetaDataTools AthenaKernel RecoToolInterfaces EventPrimitives
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasROOT GaudiInterface TrkVertexAnalysisUtils xAODEventInfo AthenaPoolUtilities xAODPrimitives CaloEvent CaloInterface TrackToCalo TrkCaloExtension InDetBeamSpotService)

atlas_add_component( DerivationFrameworkBPhys
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODMuon DataModel AthenaBaseComps AtlasBoost DerivationFrameworkInterfaces JpsiUpsilonTools xAODTracking xAODBPhys IOVDbMetaDataTools AthenaKernel RecoToolInterfaces AtlasROOT GaudiInterface TrkVertexAnalysisUtils xAODEventInfo AthenaPoolUtilities xAODPrimitives CaloEvent CaloInterface TrackToCalo TrkCaloExtension  DerivationFrameworkBPhysLib EventPrimitives InDetBeamSpotService)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
