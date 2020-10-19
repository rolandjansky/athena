################################################################################
# Package: SCT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( SCT_Digitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CommissionEvent AthenaBaseComps AthenaKernel CxxUtils PileUpToolsLib Identifier xAODEventInfo GaudiKernel SiDigitization InDetRawData InDetSimEvent HitManagement GeneratorObjects SiPropertiesToolLib InDetIdentifier InDetReadoutGeometry SCT_ReadoutGeometry InDetSimData InDetConditionsSummaryService PathResolver SCT_ConditionsToolsLib SCT_ModuleDistortionsLib )

atlas_add_test( SCT_DigitizationMT_test
                SCRIPT Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/HITS.04919495._001041.pool.root.1 --conditionsTag default:OFLCOND-RUN12-SDR-25 --digiSeedOffset1 170 --digiSeedOffset2 170 --geometryVersion ATLAS-R2-2015-03-01-00 --DataRunNumber 222525 --outputRDOFile mc15_2015_ttbar.RDO.pool.root --preInclude HITtoRDO:SimulationJobOptions/preInclude.SCTOnlyConfig.py,Digitization/ForceUseOfAlgorithms.py --postInclude Digitization/FixDataDependenciesForMT.py --skipEvents 0  --maxEvents 100 --athenaopts=--threads=10
                PROPERTIES TIMEOUT 1200
                ENVIRONMENT THREADS=10 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

