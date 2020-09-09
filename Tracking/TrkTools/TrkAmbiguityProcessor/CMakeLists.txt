################################################################################
# Package: TrkAmbiguityProcessor
################################################################################

# Declare the package name:
atlas_subdir( TrkAmbiguityProcessor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel			  
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/TrkCaloClusterROI
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkValInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces          )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library(   TrkAmbiguityProcessorLib
                     src/DenseEnvironmentsAmbiguityProcessorTool.cxx
                     src/DenseEnvironmentsAmbiguityScoreProcessorTool.cxx
                     src/AmbiguityProcessorUtility.cxx
                     src/AmbiguityProcessorBase.cxx
                     PUBLIC_HEADERS TrkAmbiguityProcessor
                     PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel AthenaBaseComps AtlasDetDescr GaudiKernel InDetPrepRawData InDetRecToolInterfaces TrkDetElementBase TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData TrkFitterInterfaces TrkToolInterfaces TrkValInterfaces TrkExInterfaces TrkCaloClusterROI)

atlas_add_component( TrkAmbiguityProcessor
                     src/SimpleAmbiguityProcessorTool.cxx  src/TrackScoringTool.cxx  src/TrackSelectionProcessorTool.cxx src/AmbiguityProcessorUtility.cxx src/AmbiguityProcessorBase.cxx
                     src/components/*.cxx 
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} TrkAmbiguityProcessorLib )

