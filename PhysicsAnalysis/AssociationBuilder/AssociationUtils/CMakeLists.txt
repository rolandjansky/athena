################################################################################
# Package: AssociationUtils
################################################################################

# Declare the package name:
atlas_subdir( AssociationUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/Navigation
                          Control/SGTools
                          Event/FourMom
                          Event/FourMomUtils
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( AssociationUtils
                   src/*.cxx
                   PUBLIC_HEADERS AssociationUtils
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES EventKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthLinks FourMomUtils )

