################################################################################
# Package: LArCafJobs
################################################################################

# Declare the package name:
atlas_subdir( LArCafJobs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloInterface
                          DetectorDescription/Identifier
                          External/AtlasCLHEP
                          External/AtlasHepMC
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecUtils
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArTools
                          LArCalorimeter/LArCabling
			   LArCalorimeter/LArBadChannelTool
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/egamma/egammaEvent
                          Tools/PyJobTransformsCore
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DataQuality/DataQualityUtils
                          Event/xAOD/xAODEventInfo
                          Event/NavFourMom
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArCondUtils
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArGeoModel/LArHV
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( HepMC )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

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
                   PUBLIC_HEADERS LArCafJobs
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps SGTools GaudiKernel LArIdentifier LArRawConditions LArRawEvent egammaEvent TrigSteeringEvent McParticleEvent StoreGateLib SGtests LArRecUtilsLib LArToolsLib TrigDecisionToolLib CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result ${HEPMC_LIBRARIES} )

atlas_add_component( LArCafJobs
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps SGTools StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent egammaEvent LArRecUtilsLib LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_dictionary( LArCafJobsDict
                      LArCafJobs/LArCafJobsDict.h
                      LArCafJobs/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps SGTools StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRecUtilsLib LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_executable( LArQuickHistMerge
                      src/LArQuickHistMerge.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps SGTools StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRecUtilsLib LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_executable( LArSamplesMerge
                      src/LArSamplesMerge.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps SGTools StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRecUtilsLib LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/LArHistMerge_trf.py share/LArCAF_tf.py share/LArNoiseBursts_tf.py share/LArNoiseBursts_fromraw_tf.py )

