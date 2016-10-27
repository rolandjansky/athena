################################################################################
# Package: InDetBeamSpotFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetBeamSpotFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
)

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit )

# Component(s) in the package:
atlas_add_library( InDetBeamSpotFinderLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetBeamSpotFinder
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventInfo xAODEventInfo TrkEventPrimitives VxVertex AtlasDetDescr Identifier TrkFitterInterfaces TrkFitterUtils TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkVertexFitterInterfaces  )

atlas_add_component( InDetBeamSpotFinder
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} xAODTracking GaudiKernel AthenaBaseComps EventInfo xAODEventInfo TrkEventPrimitives VxVertex AtlasDetDescr Identifier TrkFitterInterfaces TrkFitterUtils TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkVertexFitterInterfaces  InDetBeamSpotFinderLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

