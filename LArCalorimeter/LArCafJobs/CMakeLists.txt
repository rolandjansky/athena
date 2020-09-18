################################################################################
# Package: LArCafJobs
################################################################################

# Declare the package name:
atlas_subdir( LArCafJobs )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )


# Component(s) in the package:
atlas_add_library( LArCafJobsLib
                   src/AbsShape.cxx
                   src/CaloId.cxx
                   src/CellInfo.cxx
                   src/ClassCounts.cxx
                   src/DataContainer.cxx
                   src/DataStore.cxx
                   src/Definitions.cxx
                   src/EventData.cxx
                   src/Geometry.cxx
                   src/HistoryContainer.cxx
                   src/LArShapeDumper.cxx
                   src/LArShapeDumperTool.cxx
                   src/LArSimpleShapeDumper.cxx
                   src/PersistentAccessor.cxx
                   src/RunData.cxx
                   src/ShapeInfo.cxx
                   src/SimpleShape.cxx
                   src/LArNoiseBursts.cxx
                   src/LArHECNoise.cxx
                   src/LArReadCells.cxx
                   PUBLIC_HEADERS LArCafJobs
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps AthenaKernel GaudiKernel LArIdentifier LArRawConditions LArRawEvent egammaEvent TrigSteeringEvent McParticleEvent StoreGateLib SGtests LArToolsLib TrigDecisionToolLib CaloDetDescrLib AthAnalysisToolsLib TrigAnalysisInterfaces CaloInterfaceLib 
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result AtlasHepMCLib LArRecEvent )

atlas_add_component( LArCafJobs
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent egammaEvent LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_dictionary( LArCafJobsDict
                      LArCafJobs/LArCafJobsDict.h
                      LArCafJobs/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_executable( LArQuickHistMerge
                      src/LArQuickHistMerge.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_executable( LArSamplesMerge
                      src/LArSamplesMerge.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_scripts( share/LArHistMerge_trf.py share/LArCAF_tf.py share/LArNoiseBursts_tf.py share/LArNoiseBursts_fromraw_tf.py )
