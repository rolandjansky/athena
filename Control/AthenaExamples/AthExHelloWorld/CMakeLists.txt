# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExHelloWorld )

# Component(s) in the package:
atlas_add_component( AthExHelloWorld
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/HelloWorldConfig.py 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package:
atlas_add_test( AthExHelloWorld
   ENVIRONMENT THREADS=0
   SCRIPT test/test_AthExHelloWorld.sh )

atlas_add_test( AthExHelloWorldMT_1
   ENVIRONMENT THREADS=1
   SCRIPT test/test_AthExHelloWorld.sh )

atlas_add_test( AthExHelloWorldMT_2
   ENVIRONMENT THREADS=2
   SCRIPT test/test_AthExHelloWorld.sh
   LOG_IGNORE_PATTERN "AthenaHiveEventLoopMgr.* processing event|^HelloWorld .*(INFO|WARNING A WARNING|ERROR An ERROR|FATAL A FATAL)|my message to the world" #processing order can change
   )

atlas_add_test( AthExHelloWorld_CfgTest
   SCRIPT python -m AthExHelloWorld.HelloWorldConfig
   POST_EXEC_SCRIPT nopost.sh )
