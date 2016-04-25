# $Id: CMakeLists.txt 742521 2016-04-25 07:29:56Z krasznaa $
################################################################################
# Package: FourMomUtils
################################################################################

# Declare the package name:
atlas_subdir( FourMomUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/CxxUtils
   Event/EventKernel
   Event/NavFourMom
   Event/xAOD/xAODBase
   Event/xAOD/xAODMissingET
   PRIVATE
   Event/FourMom )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( FourMomUtils
   FourMomUtils/*.h src/*.cxx Root/*.cxx
   PUBLIC_HEADERS FourMomUtils
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES AthContainers CxxUtils EventKernel NavFourMom xAODBase
   xAODMissingET
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} FourMom )

atlas_add_dictionary( FourMomUtilsDict
   FourMomUtils/FourMomUtilsDict.h
   FourMomUtils/selection.xml
   LINK_LIBRARIES FourMomUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
