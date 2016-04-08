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
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecUtils
                          LArCalorimeter/LArTools
                          Tools/PyJobTransformsCore
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloInterface
                          DataQuality/DataQualityUtils
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArRecConditions
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

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
                   PUBLIC_HEADERS LArCafJobs
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps SGTools GaudiKernel LArIdentifier LArRawConditions LArRawEvent TrigSteeringEvent StoreGateLib SGtests LArRecUtilsLib LArToolsLib TrigDecisionToolLib CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result )

atlas_add_component( LArCafJobs
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps SGTools StoreGateLib SGtests GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRecUtilsLib LArToolsLib TrigDecisionToolLib TrigSteeringEvent CaloDetDescrLib DataQualityUtils Identifier xAODEventInfo LArRecConditions TrigConfHLTData TrigT1Result LArCafJobsLib )

atlas_add_dictionary( LArCafJobsDict
                      LArCafJobs/LArCafJobsDict.h
                      LArCafJobs/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
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
atlas_install_scripts( share/LArHistMerge_trf.py )

