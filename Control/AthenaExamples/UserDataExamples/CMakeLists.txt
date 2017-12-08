################################################################################
# Package: UserDataExamples
################################################################################

# Declare the package name:
atlas_subdir( UserDataExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthContainers
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( UserDataExamples
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthContainers GaudiKernel )

# Install files from the package:
atlas_install_headers( UserDataExamples )
atlas_install_joboptions( share/*.py )

