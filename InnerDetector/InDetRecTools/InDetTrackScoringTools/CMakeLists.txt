################################################################################
# Package: InDetTrackScoringTools
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackScoringTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkTools/TrkToolInterfaces
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkEvent/TrkCaloClusterROI
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
                          Tools/PathResolver
			  )



# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( lwtnn )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag rootMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( InDetTrackScoringTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${LWTNN_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrkEventPrimitives TrkToolInterfaces GeoPrimitives InDetIdentifier InDetRIO_OnTrack InDetRecToolInterfaces TrkParameters TrkPseudoMeasurementOnTrack TrkTrack TrkTrackSummary TrkExInterfaces TrkCaloClusterROI MagFieldElements MagFieldConditions BeamSpotConditionsData PathResolver )

# Install files from the package:
atlas_install_headers( InDetTrackScoringTools )

