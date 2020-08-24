################################################################################
# Package: InDetPhysValMonitoring
################################################################################


# Declare the package name:
atlas_subdir( InDetPhysValMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/CxxUtils
			  DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
			  InnerDetector/InDetDetDescr/InDetIdentifier
			  InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Tracking/TrkValidation/TrkValHistUtils
                          PRIVATE
                          Control/AthToolSupport/AsgTools
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          PhysicsAnalysis/MCTruthClassifier
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces 
                          InnerDetector/InDetValidation/InDetTruthVertexValidation )
                          

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread EG )
find_package( Boost COMPONENTS unit_test_framework )
find_package( XercesC )


# Component(s) in the package:
atlas_add_library( InDetPhysValMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetPhysValMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}   ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES}  ${EIGEN_LIBRARIES} 
                   AthenaBaseComps AthenaMonitoringLib CxxUtils AtlasDetDescr xAODTracking xAODTruth GaudiKernel InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry
                   InDetTrackSelectionToolLib PATCoreLib TrkValHistUtils
                   PRIVATE_LINK_LIBRARIES AsgTools AthenaKernel GeoPrimitives EventPrimitives 
                   xAODBase xAODEventInfo xAODJet InDetPrepRawData InDetRIO_OnTrack
                   AthContainers StoreGateLib
                   MCTruthClassifierLib PathResolver TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces InDetTruthVertexValidationLib )

atlas_add_component( InDetPhysValMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES}  ${EIGEN_LIBRARIES} 
                     AthenaBaseComps AthenaMonitoringLib CxxUtils xAODTracking xAODTruth GaudiKernel PATCoreLib 
                     TrkValHistUtils AsgTools AthenaKernel AtlasDetDescr GeoPrimitives EventPrimitives 
                     xAODBase xAODEventInfo xAODJet InDetIdentifier InDetPrepRawData InDetRIO_OnTrack 
                     MCTruthClassifierLib PathResolver TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces InDetPhysValMonitoringLib)





# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/*.txt )
atlas_install_runtime( share/*.xml )
atlas_install_runtime( test/InDetPhysValMonitoring_TestConfiguration.xml )

# build postprocessing program 
atlas_add_executable( postProcessIDPVMHistos util/postProcessIDPVMHistos.cxx 
        INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
        LINK_LIBRARIES ${ROOT_LIBRARIES} InDetPhysValMonitoringLib )
        
atlas_add_test( SingleHistogramDefinition_test
                SOURCES
                test/SingleHistogramDefinition_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( HistogramDefinitionSvc_test
                SOURCES
                test/HistogramDefinitionSvc_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( XmlEntityInclusion_test
                SOURCES
                test/XmlEntityInclusion_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( InDetPlotBase_test
                SOURCES
                test/InDetPlotBase_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)


