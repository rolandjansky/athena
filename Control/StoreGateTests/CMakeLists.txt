################################################################################
# Package: StoreGateTests
################################################################################

# Declare the package name:
atlas_subdir( StoreGateTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaPython
                          Control/AthenaKernel
                          Control/AthAllocators
                          Control/AthContainers
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( StoreGateTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthAllocators AthContainers AthenaBaseComps StoreGateLib SGtests GaudiKernel )

atlas_add_dictionary( StoreGateTestsDict
                      StoreGateTests/StoreGateTestsDict.h
                      StoreGateTests/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAllocators AthContainers AthenaBaseComps StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_headers( StoreGateTests )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py share/tests/*.py )

atlas_add_test( PyClidsTestWriter
                SCRIPT test/PyClidsTestWriter.sh
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN "running|XMLCatalog" )

atlas_add_test( SgProducerConsumer
                SCRIPT test/SgProducerConsumer.sh
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN "running|XMLCatalog" )

atlas_add_test( SgProducerConsumerDataPool
                SCRIPT test/SgProducerConsumerDataPool.sh
                PROPERTIES TIMEOUT 600
                LOG_IGNORE_PATTERN "running|XMLCatalog" )
