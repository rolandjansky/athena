################################################################################
# Package: TrackD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TrackD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/AthContainers
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoAdaptors
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/InDetTestBLayer
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonIdHelpers
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
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
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TrackD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} TrkParameters CommissionEvent AthenaKernel CxxUtils AthContainers StoreGateLib SGtests AtlasDetDescr GeoAdaptors Identifier EventPrimitives xAODBase xAODEventInfo xAODPrimitives xAODTracking GaudiKernel GeneratorObjects TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib InDetRawData InDetPrepRawData InDetRIO_OnTrack InDetTestBLayerLib InDetSimEvent MuonIdHelpersLib ParticleEvent D3PDMakerUtils MCTruthClassifierLib muonEvent Particle ParticleTruth ITrackToVertex RecoToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParticleBase TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData TrkV0Vertex VxVertex TrkToolInterfaces TrkVertexFitterValidationUtilsLib TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_headers( TrackD3PDMaker )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

