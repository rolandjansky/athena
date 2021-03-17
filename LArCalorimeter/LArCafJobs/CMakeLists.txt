# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCafJobs )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Matrix Hist Graf RIO )


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
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthAnalysisToolsLib AthenaBaseComps AthenaKernel CaloConditions CaloDetDescrLib CaloIdentifier CaloInterfaceLib GaudiKernel Identifier LArCablingLib LArElecCalib LArIdentifier LArRawConditions LArRawEvent LArRecConditions StoreGateLib TrigAnalysisInterfaces TrigConfInterfaces TrigDecisionToolLib egammaEvent
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AthenaPoolUtilities CaloEvent CaloGeoHelpers LArRecEvent NavFourMom TrigConfHLTData TrigSteeringEvent TrigT1Result xAODEventInfo )

atlas_add_component( LArCafJobs
                     src/components/*.cxx
                     LINK_LIBRARIES LArCafJobsLib )

atlas_add_dictionary( LArCafJobsDict
                      LArCafJobs/LArCafJobsDict.h
                      LArCafJobs/selection.xml
                      LINK_LIBRARIES LArCafJobsLib )

atlas_add_executable( LArQuickHistMerge
                      src/LArQuickHistMerge.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataQualityUtils )

atlas_add_executable( LArSamplesMerge
                      src/LArSamplesMerge.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} LArCafJobsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_scripts( share/LArHistMerge_trf.py share/LArCAF_tf.py share/LArNoiseBursts_tf.py share/LArNoiseBursts_fromraw_tf.py )
