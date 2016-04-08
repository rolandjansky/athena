################################################################################
# Package: xAODCore
################################################################################

# Declare the package name:
atlas_subdir( xAODCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthContainersInterfaces
                          Control/AthLinks
                          Control/AthenaKernel
                          Control/SGTools
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS MathCore GenVector Cintex RIO Hist Tree Cint Core pthread )

# tag ROOTCintexLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_root_dictionary( xAODCore
                           xAODCoreDictSource
                           ROOT_HEADERS xAODCore/tools/ReadStats.h xAODCore/tools/PerfStats.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( xAODCore
                   Root/*.cxx
                   ${xAODCoreDictSource}
                   PUBLIC_HEADERS xAODCore
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( xAODCoreRflxDict
                      xAODCore/xAODCoreRflxDict.h
                      xAODCore/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaKernel SGTools TestTools xAODCore )

atlas_add_test( ut_xaodcore_auxselection_test
                SOURCES
                test/ut_xaodcore_auxselection_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaKernel SGTools TestTools xAODCore )

atlas_add_test( ut_xaodcore_printhelpers_test
                SOURCES
                test/ut_xaodcore_printhelpers_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaKernel SGTools TestTools xAODCore )

