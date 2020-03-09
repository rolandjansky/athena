################################################################################
# Package: xAODTrigL1Calo
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigL1Calo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          PRIVATE
                          Control/AthLinks )

                         
# External dependencies:
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread GenVector )

# Extra dependencies, based on what environment we are in:
if (BUILDVP1LIGHT)
    if( BUILDVP1LIGHT_DIST STREQUAL "ubuntu")
        set( extra_libs GenVector )
    endif()
endif()

# Component(s) in the package:
atlas_add_library( xAODTrigL1Calo
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigL1Calo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODBase xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks ${extra_libs} )

atlas_add_dictionary( xAODTrigL1CaloDict
                      xAODTrigL1Calo/xAODTrigL1CaloDict.h
                      xAODTrigL1Calo/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${extra_libs} AthContainers xAODBase xAODCore AthLinks xAODTrigL1Calo
                      EXTRA_FILES Root/dict/*.cxx )

