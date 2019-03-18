################################################################################
# Package: TrigT1Interfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigT1Interfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthContainers
                          GaudiKernel
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfL1Data
                          AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigT1Interfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1Interfaces
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthContainers AthenaKernel GaudiKernel ${TDAQ-COMMON_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES TrigConfL1Data )

atlas_add_dictionary( TrigT1InterfacesDict
                      TrigT1Interfaces/TrigT1InterfacesDict.h
                      TrigT1Interfaces/selection.xml
                      INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XERCESC_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthContainers AthenaKernel GaudiKernel TrigConfL1Data TrigT1Interfaces )


atlas_add_test( CPRoIDecoder_test
                SOURCES test/CPRoIDecoder_test.cxx
                LINK_LIBRARIES TrigT1Interfaces )


atlas_add_test( JEPRoIDecoder_test
                SOURCES test/JEPRoIDecoder_test.cxx
                LINK_LIBRARIES TrigT1Interfaces )
