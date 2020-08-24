# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StoreGateTests )

# Component(s) in the package:
atlas_add_component( StoreGateTests
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthAllocators AthContainers AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib )

atlas_add_dictionary( StoreGateTestsDict
                      StoreGateTests/StoreGateTestsDict.h
                      StoreGateTests/selection.xml
                      LINK_LIBRARIES AthAllocators AthContainers AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib )

# Install files from the package:
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
