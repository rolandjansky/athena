################################################################################
# Package: SCT_DigitizationRTT
################################################################################

# Declare the package name:
atlas_subdir( SCT_DigitizationRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/EventInfo
                          Event/EventPrimitives
                          GaudiKernel
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetSimEvent
                          PhysicsAnalysis/AnalysisCommon/CompositeParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_DigitizationRTT
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests EventInfo GeneratorObjects SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry InDetRawData InDetSimData InDetPrepRawData InDetSimEvent McParticleEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/SCT_DigitizationRTT_TestConfiguration.xml )

