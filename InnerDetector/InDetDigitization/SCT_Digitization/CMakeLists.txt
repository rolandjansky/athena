################################################################################
# Package: SCT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( SCT_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Commission/CommissionEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDigitization/SiDigitization
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          PRIVATE
                          Control/StoreGate
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetConditions/SiPropertiesTool
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ModuleDistortions
                          InnerDetector/InDetRawEvent/InDetSimData )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CommissionEvent AthenaBaseComps AthenaKernel PileUpToolsLib Identifier xAODEventInfo GaudiKernel SiDigitization InDetRawData InDetSimEvent HitManagement GeneratorObjects SiPropertiesToolLib InDetIdentifier InDetReadoutGeometry InDetSimData )

atlas_add_test( SCT_DigitizationConfigNew_test
                SCRIPT test/SCT_DigitizationConfigNew_test.py
                PROPERTIES TIMEOUT 300 )
atlas_add_test( SCT_DigitizationMT_test
                SCRIPT Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/HITS.04919495._001041.pool.root.1 --conditionsTag default:OFLCOND-RUN12-SDR-25 --digiSeedOffset1 170 --digiSeedOffset2 170 --geometryVersion ATLAS-R2-2015-03-01-00 --DataRunNumber 222525 --outputRDOFile mc15_2015_ttbar.RDO.pool.root --preInclude HITtoRDO:SimulationJobOptions/preInclude.SCTOnlyConfig.py,Digitization/ForceUseOfAlgorithms.py --postInclude Digitization/FixDataDependenciesForMT.py --skipEvents 0  --maxEvents 100 --athenaopts '--threads 10'
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=10 )

# Install files from the package:
atlas_install_headers( SCT_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

