###############################################################################
# Package: SCT_ConditionsAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsAlgorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelUtilities
			  DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkGeometry )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( SCT_ConditionsAlgorithms
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier DetDescrConditions GeoModelUtilities GaudiKernel SCT_ConditionsData SCT_CablingLib AthenaPoolUtilities InDetIdentifier InDetReadoutGeometry TrkGeometry SCT_ConditionsToolsLib )

atlas_add_test( TestCalibChipRead
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testCalibChipRead.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestCalibRead
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testCalibRead.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestConfig
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testConfig.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestDCSConditions
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testDCSConditions.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestLinkMasking
                SCRIPT share/TestLinkMasking.sh
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestMajority
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testMajority.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestModuleVeto
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testModuleVeto.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestMonRead
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testMonRead.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestParameters
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testParameters.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestReadout
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testReadout.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestRodVeto
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testRodVeto.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestSensors
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testSensors.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestSilicon
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testSilicon.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestStripVeto
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testStripVeto.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestSummary
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testSummary.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestTdaqEnabled
                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testTdaqEnabled.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.sh )
