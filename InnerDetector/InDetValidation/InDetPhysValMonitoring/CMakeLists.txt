################################################################################
# Package: InDetPhysValMonitoring
################################################################################


# Declare the package name:
atlas_subdir( InDetPhysValMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
			  DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
			  InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Tracking/TrkValidation/TrkValHistUtils
			  DetectorDescription/AtlasDetDescr
                          PRIVATE
                          Control/AthToolSupport/AsgTools
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          PhysicsAnalysis/MCTruthClassifier
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )
                          

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread EG )
find_package( XercesC )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( InDetPhysValMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaMonitoringLib xAODTracking xAODTruth GaudiKernel PATCoreLib TrkValHistUtils AsgTools AthenaKernel AtlasDetDescr GeoPrimitives EventPrimitives TRT_ReadoutGeometry SCT_ReadoutGeometry PixelReadoutGeometry xAODBase xAODEventInfo xAODJet InDetIdentifier InDetPrepRawData InDetRIO_OnTrack MCTruthClassifierLib PathResolver TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetPhysValMonitoring )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.py share/*.hdef  share/*.xml  share/*.xsl} )
atlas_install_runtime( test/InDetPhysValMonitoring_TestConfiguration.xml )

