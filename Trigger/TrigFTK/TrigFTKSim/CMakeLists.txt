################################################################################
# Package: TrigFTKSim
################################################################################ 

# Declare the package name:
atlas_subdir( TrigFTKSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/PileUpTools
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/Particle
                          Tools/PyJobTransforms
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTools/TrkTrackSummaryTool
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigFTK/TrigFTKPool
                          PRIVATE
                          DetectorDescription/IdDictDetDescr
                          Event/EventInfo
                          Event/NavFourMom
                          Generators/GeneratorObjects
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Trigger/TrigTools/TrigFTKToolInterfaces )

# External dependencies:
find_package( Boost COMPONENTS iostreams program_options filesystem thread system )
find_package( Eigen )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Matrix Cint Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( TBB )

add_definitions(-DHAVE_BOOST_IOSTREAMS)

# this line failed automatic conversion in cmt2cmake :
# action makeTrfSignatures "makeTrfSignatures.py --output ../share/$(package)Signatures.json"

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_root_dictionary( TrigFTKSimLib
                           TrigFTKSimLibDictSource
                           ROOT_HEADERS TrigFTKSim/FTKRoad.h TrigFTKSim/FTKTruthTrack.h TrigFTKSim/FTKRawHit.h TrigFTKSim/FTKHit.h TrigFTKSim/FTKTrack.h TrigFTKSim/FTKSSMap.h TrigFTKSim/FTKRoadStream.h TrigFTKSim/FTKTrackStream.h TrigFTKSim/FTKSS.h TrigFTKSim/MultiTruth.h TrigFTKSim/FTKPattern.h TrigFTKSim/TrigFTKSimLinkDef.h
                           EXTERNAL_PACKAGES  ROOT Boost HepPDT Eigen HepMC TBB )

atlas_add_library( TrigFTKSimLib
                   src/FTKRoadFinderAlgo.cxx
                   src/FTKDumpCondAlgo.cxx
                   src/RoadFinder.cxx
                   src/FTK_SGHitInput.cxx
                   src/FTKDetectorTool.cxx
                   src/FTK_SGRoadOutput.cxx
                   src/FTK_SGTrackOutput.cxx
                   src/FTK_RoadMarketTool.cxx
                   src/FTK_RoadMarketIO.cxx
                   src/FTK_RegionalRawInput.cxx
                   src/FTKSplitEncoder.cxx
                   src/FTKSetup.cxx
                   src/PatternBank.cxx
                   src/FTK_AMBank.cxx
                   src/atlClustering.cxx
                   src/FTKRoadFileOutput.cxx
                   src/FTKConstantBank.cxx
                   src/FTKTrackInput.cxx
                   src/FTKRoadFileInput.cxx
                   src/FTK_RawInput.cxx
                   src/FTKTruthTrack.cxx
                   src/FTKRawHit.cxx
                   src/FTKDataInput.cxx
                   src/ftkdefs.cxx
                   src/FTKPMap.cxx
                   src/FTKRegionMap.cxx
                   src/FTKSectorMap.cxx
                   src/FTKSSMap.cxx
                   src/ftk_dcap.cxx
                   src/FTKRoad.cxx
                   src/FTKHit.cxx
                   src/FTKTrack.cxx
                   src/FTKRoadStream.cxx
                   src/FTKRoadKDTree.cxx
                   src/FTKTrackStream.cxx
                   src/FTKSS.cxx
                   src/MultiTruth.cxx
                   src/FTKPattern.cxx
                   src/tsp/*.cxx
                   src/FTKMergerAlgo.cxx
                   src/TrackFitter.cxx
                   src/TrackFitter711.cxx
                   src/FTKSector711DB.cxx
                   src/FTKTrackFileOutput.cxx
                   src/FTKTrackFitterAlgo.cxx
                   src/FTKLogging.cxx
                   src/FTKPatternOneSector.cxx
                   src/FTKPatternBySector.cxx
                   src/FTKPatternWithCoverage.cxx
                   src/FTKRootFile.cxx
                   src/FTKMergeRoot.cxx
                   src/FTK_AMsimulation_base.cxx
                   src/FTK_CompressedAMBank.cxx
                   src/FTKSteering.cxx
                   ${TrigFTKSimLibDictSource}
                   PUBLIC_HEADERS TrigFTKSim
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps GaudiKernel InDetCondTools InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkParameters TrkParticleBase TrkTrack TrkTruthData TrkExInterfaces TrkToolInterfaces TrigInDetEvent TrigFTKPool PileUpToolsLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} IdDictDetDescr EventInfo NavFourMom GeneratorObjects InDetRawData InDetSimData TrkMeasurementBase TrkRIO_OnTrack )

atlas_add_component( TrigFTKSim
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps PileUpToolsLib StoreGateLib SGtests GaudiKernel InDetCondTools InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkParameters TrkParticleBase TrkTrack TrkTruthData TrkExInterfaces TrkToolInterfaces TrigInDetEvent TrigFTKPool IdDictDetDescr EventInfo NavFourMom GeneratorObjects InDetRawData InDetSimData TrkMeasurementBase TrkRIO_OnTrack TrigFTKSimLib )


atlas_add_executable( trigftk_partitionbalancing
                      standalone/partitionbalancing.cc
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps PileUpToolsLib StoreGateLib SGtests GaudiKernel InDetCondTools InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkParameters TrkParticleBase TrkTrack TrkTruthData TrkExInterfaces TrkToolInterfaces TrigInDetEvent TrigFTKPool IdDictDetDescr EventInfo NavFourMom GeneratorObjects InDetRawData InDetSimData TrkMeasurementBase TrkRIO_OnTrack TrigFTKSimLib )

atlas_add_executable( trigftk_efficiency
                      standalone/efficiency.cc
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps PileUpToolsLib StoreGateLib SGtests GaudiKernel InDetCondTools InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkParameters TrkParticleBase TrkTrack TrkTruthData TrkExInterfaces TrkToolInterfaces TrigInDetEvent TrigFTKPool IdDictDetDescr EventInfo NavFourMom GeneratorObjects InDetRawData InDetSimData TrkMeasurementBase TrkRIO_OnTrack TrigFTKSimLib )

atlas_add_executable( trigftk_dataflow
                      standalone/dataflow.cc
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps PileUpToolsLib StoreGateLib SGtests GaudiKernel InDetCondTools InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkParameters TrkParticleBase TrkTrack TrkTruthData TrkExInterfaces TrkToolInterfaces TrigInDetEvent TrigFTKPool IdDictDetDescr EventInfo NavFourMom GeneratorObjects InDetRawData InDetSimData TrkMeasurementBase TrkRIO_OnTrack TrigFTKSimLib )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/TrigFTKTruthAlgsConfig.py python/QueryFTKdb.py python/findInputs.py python/FTKSimOptions.py )
atlas_install_joboptions( share/*.py )
atlas_install_joboptions( share/skeleton.*.py )
atlas_install_runtime( scripts/*_tf.py )
atlas_install_runtime( test/TrigFTKSim_TestConfiguration.xml scripts/rtt_compare.py scripts/TrigFTKSM4Un_tf.py scripts/TrigFTKTM64SM4Un_tf.py scripts/TrigFTKSimITk_tf.py )
atlas_install_generic( config/map_file/*
                       DESTINATION share/ftk_configuration/map_files
                       EXECUTABLE )
atlas_install_generic( config/ss_file/*
                       DESTINATION share/ftk_configuration/ss_files
                       EXECUTABLE )
atlas_install_generic( config/hwsdev_file/*
                       DESTINATION share/ftk_configuration/hwsdev_file
                       EXECUTABLE )
atlas_install_generic( config/bad_modules/*
                       DESTINATION share/ftk_configuration/bad_modules
                       EXECUTABLE )
