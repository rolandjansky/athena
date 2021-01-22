################################################################################
# Package: TrkAmbiguityProcessor
################################################################################

# Declare the package name:
atlas_subdir( TrkAmbiguityProcessor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/SGTools
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
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkValInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces          )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library(   TrkAmbiguityProcessorLib
                     src/DenseEnvironmentsAmbiguityProcessorTool.cxx
                     src/SimpleFitterTool.cxx
                     PUBLIC_HEADERS TrkAmbiguityProcessor
                     PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AtlasDetDescr GaudiKernel InDetPrepRawData InDetRecToolInterfaces TrkDetElementBase TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkFitterInterfaces TrkToolInterfaces TrkValInterfaces TrkExInterfaces )

atlas_add_component( TrkAmbiguityProcessor
                     src/SimpleAmbiguityProcessorTool.cxx  src/TrackScoringTool.cxx  src/TrackSelectionProcessorTool.cxx
                     src/components/*.cxx 
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkAmbiguityProcessorLib )

# Install files from the package:
atlas_install_headers( TrkAmbiguityProcessor )

