# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1Interfaces )

# External dependencies:
find_package( XercesC )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigT1Interfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1Interfaces
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${XERCESC_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES TrigConfL1Data )

atlas_add_dictionary( TrigT1InterfacesDict
                      TrigT1Interfaces/TrigT1InterfacesDict.h
                      TrigT1Interfaces/selection.xml
                      LINK_LIBRARIES TrigT1Interfaces )

# Tests in the package:
atlas_add_test( CPRoIDecoder_test
                SOURCES test/CPRoIDecoder_test.cxx
                LINK_LIBRARIES TrigT1Interfaces TestTools )

atlas_add_test( JEPRoIDecoder_test
                SOURCES test/JEPRoIDecoder_test.cxx
                LINK_LIBRARIES TrigT1Interfaces TestTools )
