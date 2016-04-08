################################################################################
# Package: JetValidation
################################################################################

# Declare the package name:
atlas_subdir( JetValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( JetValidation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

# Install files from the package:
atlas_install_headers( JetValidation )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/JetValidation_TestConfiguration.xml )
atlas_install_scripts( scripts/jetMakeRefSamples.py )

