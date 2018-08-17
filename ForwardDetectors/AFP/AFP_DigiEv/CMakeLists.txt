################################################################################
# Package: AFP_DigiEv
################################################################################

# Declare the package name:
atlas_subdir( AFP_DigiEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Simulation/HitManagement )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AFP_DigiEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_DigiEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AFP_DigiEvDict
                      AFP_DigiEv/AFP_DigiEvDict.h
                      AFP_DigiEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers HitManagement AFP_DigiEv )

