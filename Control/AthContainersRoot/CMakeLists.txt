#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthContainersRoot )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_library( AthContainersRoot
                   src/*.cxx
                   PUBLIC_HEADERS AthContainersRoot
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks CxxUtils RootUtils
                   LINK_LIBRARIES AthenaKernel GaudiKernel AthContainersInterfaces )


atlas_add_dictionary( AthContainersRootTestDict
   AthContainersRoot/AthContainersRootTestDict.h
   AthContainersRoot/selection.xml
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainersRoot AthContainers )


atlas_add_test( RootAuxVectorFactory_test
                SOURCES
                test/RootAuxVectorFactory_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers CxxUtils RootUtils AthContainersRoot )


atlas_add_test( getDynamicAuxID_test
                SOURCES
                test/getDynamicAuxID_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers CxxUtils RootUtils AthContainersRoot )


atlas_add_test( AuxStoreRoot_test
                SOURCES
                test/AuxStoreRoot_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers CxxUtils RootUtils AthContainersRoot TestTools )

