################################################################################
# Package: RootAuxDynIO
################################################################################

# Declare the package name:
atlas_subdir( RootAuxDynIO )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Control/AthContainers
                          Control/AthContainersRoot
                          Control/AthContainersInterfaces
                          Control/AthenaRootComps
                          GaudiKernel )

# External dependencies:
#find_package( ROOT COMPONENTS Core RIO TreePlayer Tree MathCore Hist pthread )


# Component(s) in the package:
atlas_add_library( RootAuxDynIO
                   src/*.cxx
                   PUBLIC_HEADERS RootAuxDynIO
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} 
                   PRIVATE_LINK_LIBRARIES TestTools AthContainers AthContainersRoot RootUtils GaudiKernel )
