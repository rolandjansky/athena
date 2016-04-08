################################################################################
# Package: HIEventUtils
################################################################################

# Declare the package name:
atlas_subdir( HIEventUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODForward
                          Event/xAOD/xAODHIEvent
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( HIEventUtilsLib
                   Root/*.cxx
                   PUBLIC_HEADERS HIEventUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AsgTools xAODForward xAODHIEvent GaudiKernel
                   PRIVATE_LINK_LIBRARIES CxxUtils )

atlas_add_component( HIEventUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AsgTools xAODForward xAODHIEvent GaudiKernel CxxUtils HIEventUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

