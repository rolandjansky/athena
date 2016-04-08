################################################################################
# Package: FourMomUtils
################################################################################

# Declare the package name:
atlas_subdir( FourMomUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/CxxUtils
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMissingET )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FourMomUtils
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS FourMomUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers CxxUtils EventKernel FourMom NavFourMom xAODBase xAODMissingET
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( FourMomUtilsDict
                      FourMomUtils/FourMomUtilsDict.h
                      FourMomUtils/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers CxxUtils EventKernel FourMom NavFourMom xAODBase xAODMissingET FourMomUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )

