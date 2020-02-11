################################################################################
# Package: TrigCompositeUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigCompositeUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Control/AthContainers
                          Control/AthLinks 
                          PRIVATE
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODEgamma
                          AtlasTest/TestTools
                          Control/StoreGate
                          )

atlas_add_library( TrigCompositeUtilsLib
                   Root/*.cxx
                   PUBLIC_HEADERS TrigCompositeUtils
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils GaudiKernel xAODTrigger )

atlas_install_python_modules( python/*.py )

atlas_add_executable( trigconf_string2hash
   util/trigconf_string2hash.cxx
   LINK_LIBRARIES TrigCompositeUtilsLib )

# Unit tests:
atlas_add_test( TrigCompositeUtils_test
                SOURCES test/TrigCompositeUtils_test.cxx
                LINK_LIBRARIES TestTools xAODTrigger TrigCompositeUtilsLib AthContainers )

atlas_add_test( TrigTraversal_test
                SOURCES test/TrigTraversal_test.cxx
                LINK_LIBRARIES  TestTools xAODTrigger xAODEgamma xAODMuon xAODBase TrigCompositeUtilsLib
                AthContainers )

atlas_add_test( Combinators_test
                SOURCES test/Combinators_test.cxx
                LINK_LIBRARIES TestTools xAODTrigger TrigCompositeUtilsLib AthContainers )

atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*.py
   POST_EXEC_SCRIPT nopost.sh )
