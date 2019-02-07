################################################################################
# Package: AthExHelloWorld
################################################################################

# Declare the package name:
atlas_subdir( AthExHelloWorld )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
			  Control/AthenaConfiguration
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( AthExHelloWorld
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps )

# Install files from the package:
atlas_install_headers( AthExHelloWorld )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/HelloWorldConfig.py )

atlas_add_test( AthExHelloWorld
    ENVIRONMENT THREADS=0
		SCRIPT test/test_AthExHelloWorld.sh
		)

atlas_add_test( AthExHelloWorldMT_1
    ENVIRONMENT THREADS=1
		SCRIPT test/test_AthExHelloWorld.sh
		)

atlas_add_test( AthExHelloWorldMT_2
    ENVIRONMENT THREADS=2
		SCRIPT test/test_AthExHelloWorld.sh
    EXTRA_PATTERNS "AthenaHiveEventLoopMgr.* processing event|^HelloWorld .*(INFO|WARNING A WARNING|ERROR An ERROR|FATAL A FATAL" #processing order can change
		)

atlas_add_test( AthExHelloWorld_CfgTest    SCRIPT python -m AthExHelloWorld.HelloWorldConfig    POST_EXEC_SCRIPT nopost.sh )
