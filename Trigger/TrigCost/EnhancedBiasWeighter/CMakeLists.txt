################################################################################
# Package: EnhancedBiasWeighter
################################################################################

# Declare the package name:
atlas_subdir( EnhancedBiasWeighter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
                          AtlasTest/TestTools
                          Tools/PathResolver )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS XMLIO XMLParser Core Tree MathCore Hist RIO pthread )
find_package( xml )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( EnhancedBiasWeighterLib
                   Root/*.cxx
                   PUBLIC_HEADERS EnhancedBiasWeighter
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} AsgTools xAODEventInfo GaudiKernel ${XML_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} TestTools PathResolver )

atlas_add_component( EnhancedBiasWeighter
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${XML_LIBRARIES} AsgTools xAODEventInfo GaudiKernel TestTools PathResolver EnhancedBiasWeighterLib )

atlas_add_dictionary( EnhancedBiasWeighterDict
                      EnhancedBiasWeighter/EnhancedBiasWeighterDict.h
                      EnhancedBiasWeighter/selection.xml
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${XML_LIBRARIES} AsgTools xAODEventInfo GaudiKernel TestTools PathResolver EnhancedBiasWeighterLib )

# No tests at the moment
#atlas_add_test( ut_EnhancedBiasWeighter_test
#                SOURCES
#                test/ut_EnhancedBiasWeighter_test.cxx
#                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
#                LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${XML_LIBRARIES} AsgTools xAODEventInfo GaudiKernel TestTools PathResolver EnhancedBiasWeighterLib )

