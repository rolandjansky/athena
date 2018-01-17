################################################################################
# Package: AthenaPoolTestData
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolTestData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthenaPoolTestData
                   src/IAthenaPoolTestData.cxx
                   src/AthenaPoolTestA.cxx
                   src/AthenaPoolTestB.cxx
                   src/AthenaPoolTestC.cxx
                   src/AthenaPoolTestD.cxx
                   src/AthenaPoolTestMatrix.cxx
                   src/AthenaPoolTestMap.cxx
                   src/AthenaPoolTestMapData.cxx
                   src/FauxTriggerMap.cxx
                   src/TestGaudiProperty.cxx
                   PUBLIC_HEADERS AthenaPoolTestData
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel AthLinks AthContainers GaudiKernel InDetRawData
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AthenaPoolTestDataDict
                      AthenaPoolTestData/AthenaPoolTestDataDict.h
                      AthenaPoolTestData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel AthLinks AthContainers GaudiKernel InDetRawData AthenaPoolTestData )

