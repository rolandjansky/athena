################################################################################
# Package: InDetPerformanceMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetPerformanceMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/Jet/JetInterface
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/egamma/egammaEvent
                          Tools/LWHists
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          DetectorDescription/IdDictDetDescr
                          Event/EventPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/egamma/egammaInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/TrkV0Vertex
                          Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
                          Trigger/TrigAnalysis/TrigDecisionTool )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( InDetPerformanceMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetPerformanceMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODTracking xAODTruth GaudiKernel GeneratorObjects InDetReadoutGeometry InDetRIO_OnTrack JetInterface ITrackToVertex egammaEvent LWHists TrkParameters TrkParticleBase TrkTrack AthenaMonitoringLib StoreGateLib SGtests ElectronPhotonSelectorToolsLib TrkVertexAnalysisUtilsLib TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} IdDictDetDescr xAODEventInfo EventPrimitives InDetIdentifier InDetPrepRawData TrkEventPrimitives TrkTrackSummary TrkTruthData TrkV0Vertex )

atlas_add_component( InDetPerformanceMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringLib StoreGateLib SGtests xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODTracking xAODTruth GaudiKernel GeneratorObjects InDetReadoutGeometry InDetRIO_OnTrack ElectronPhotonSelectorToolsLib JetInterface ITrackToVertex egammaEvent LWHists TrkParameters TrkParticleBase TrkTrack IdDictDetDescr xAODEventInfo EventPrimitives InDetIdentifier InDetPrepRawData TrkEventPrimitives TrkTrackSummary TrkTruthData TrkV0Vertex TrkVertexAnalysisUtilsLib TrigDecisionToolLib InDetPerformanceMonitoringLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

