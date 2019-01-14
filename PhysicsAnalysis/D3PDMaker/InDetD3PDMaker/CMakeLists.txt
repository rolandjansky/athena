################################################################################
# Package: InDetD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( InDetD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Commission/CommissionEvent
                          Control/AthContainers
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoAdaptors
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetSimEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/ParticleTruth
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/TrkV0Vertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkVertexFitterValidationUtils
                          )

# External dependencies:
find_package( Boost )
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( InDetD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPMC_LIBRARIES} ${HEPPDT_LIBRARIES}  GaudiKernel CommissionEvent AthContainers AthenaKernel StoreGateLib AtlasDetDescr GeoAdaptors Identifier EventPrimitives xAODEventInfo xAODTracking InDetBeamSpotService SCT_ConditionsTools TRT_ConditionsServices InDetIdentifier InDetReadoutGeometry SCT_CablingLib InDetRawData InDetPrepRawData InDetRIO_OnTrack InDetSimEvent D3PDMakerUtils MCTruthClassifier ParticleTruth ITrackToVertex muonEvent Particle TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkParticleBase TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData TrkV0Vertex VxVertex TrkToolInterfaces TrkVertexFitterValidationUtils )

# Install files from the package:
atlas_install_headers( InDetD3PDMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
