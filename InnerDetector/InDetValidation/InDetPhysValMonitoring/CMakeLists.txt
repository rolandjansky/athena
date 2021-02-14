# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetPhysValMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread EG )
find_package( Boost COMPONENTS unit_test_framework )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( InDetPhysValMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetPhysValMonitoring
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaMonitoringLib AtlasDetDescr BeamSpotConditionsData CxxUtils GaudiKernel InDetIdentifier InDetTrackSelectionToolLib InDetTruthVertexValidationLib MCTruthClassifierLib TrkValHistUtils TRT_ReadoutGeometry xAODBase xAODEventInfo xAODJet xAODTracking xAODTruth InDetTrackSystematicsToolsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} AsgTools AthContainers EventPrimitives GeoPrimitives InDetPrepRawData InDetRIO_OnTrack PATCoreAcceptLib PathResolver StoreGateLib TrkEventPrimitives TrkExInterfaces TrkParameters TrkSurfaces TrkToolInterfaces TrkTrack )

atlas_add_component( InDetPhysValMonitoring
                     src/components/*.cxx
                     LINK_LIBRARIES InDetPhysValMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/*.txt )
atlas_install_runtime( share/*.xml share/*.xsd)
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
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils TestTools InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( XmlEntityInclusion_test
                SOURCES
                test/XmlEntityInclusion_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils TestTools InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( InDetPlotBase_test
                SOURCES
                test/InDetPlotBase_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils TestTools xAODTracking InDetPhysValMonitoringLib
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( ValidateXml_test
                SCRIPT "xmllint ${CMAKE_CURRENT_SOURCE_DIR}/share/InDetPVMPlotDefRun2.xml --schema ${CMAKE_CURRENT_SOURCE_DIR}/share/hdefSchema.xsd --noent --noout"
                POST_EXEC_SCRIPT "nopost.sh" 
)


atlas_add_test( StandaloneRun_test 
                SCRIPT test/IDPVM_standalone_test.sh 
                POST_EXEC_SCRIPT "nopost.sh"                
                PROPERTIES TIMEOUT 900
)


