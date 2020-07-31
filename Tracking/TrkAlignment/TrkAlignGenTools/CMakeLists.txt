################################################################################
# Package: TrkAlignGenTools
################################################################################

# Declare the package name:
atlas_subdir( TrkAlignGenTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetAlignTools/InDetAlignGenTools
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkAlignment/TrkAlignInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkFitter/TrkFitterUtils
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/Particle
                          Tracking/TrkAlignment/TrkAlgebraUtils
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkVertexOnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# tag ROOTMinuitLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TrkAlignGenToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlignGenTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives EventPrimitives xAODTracking GaudiKernel muonEvent TrkAlignEvent TrkEventPrimitives VxVertex TrkFitterUtils StoreGateLib SGtests TrkAlignInterfaces
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AtlasDetDescr xAODEventInfo MuonSegment MuonRecToolInterfaces Particle TrkAlgebraUtils TrkDetElementBase TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkVertexOnTrack TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces TrkVertexFitterInterfaces BeamSpotConditionsData InDetTrackSelectionToolLib InDetAlignGenToolsLib )

atlas_add_component( TrkAlignGenTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives EventPrimitives xAODTracking GaudiKernel muonEvent TrkAlignEvent TrkEventPrimitives VxVertex TrkFitterUtils StoreGateLib SGtests AtlasDetDescr xAODEventInfo MuonSegment MuonRecToolInterfaces Particle TrkAlgebraUtils TrkDetElementBase TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkVertexOnTrack TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces TrkVertexFitterInterfaces BeamSpotConditionsData TrkAlignGenToolsLib InDetAlignGenToolsLib InDetTrackSelectionToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

