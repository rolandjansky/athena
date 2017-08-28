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
atlas_install_python_modules( share/HelloWorldConfig.py )
atlas_install_runtime( test/AthExHelloWorld_TestConfiguration.xml )

