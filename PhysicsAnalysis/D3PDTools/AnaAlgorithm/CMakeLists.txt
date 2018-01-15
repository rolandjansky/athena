
# The name of the package:
atlas_subdir( AnaAlgorithm )

# Find the needed external(s):
find_package( ROOT COMPONENTS Core Hist )

# The package's dependencies:
if( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      PhysicsAnalysis/D3PDTools/RootCoreUtils
      Control/AthToolSupport/AsgTools )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthenaBaseComps
      Control/AthToolSupport/AsgTools
      PRIVATE
      PhysicsAnalysis/D3PDTools/RootCoreUtils )
endif()

# The main library of the package:
if( XAOD_STANDALONE )
   atlas_add_library( AnaAlgorithmLib
      AnaAlgorithm/*.h AnaAlgorithm/*.icc Root/*.cxx
      PUBLIC_HEADERS AnaAlgorithm
      PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES AsgTools RootCoreUtils
      PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )
else()
   atlas_add_library( AnaAlgorithmLib
      AnaAlgorithm/Global.h Root/Global.cxx
      AnaAlgorithm/AnaAlgorithm.h AnaAlgorithm/AnaAlgorithm.icc
      Root/AnaAlgorithm.cxx
      AnaAlgorithm/MessageCheck.h Root/MessageCheck.cxx
      PUBLIC_HEADERS AnaAlgorithm
      PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES AthenaBaseComps AsgTools
      PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} RootCoreUtils )
endif()

# Dictionary for the package's types:
atlas_add_dictionary( AnaAlgorithmDict
   AnaAlgorithm/AnaAlgorithmDict.h
   AnaAlgorithm/selection.xml
   LINK_LIBRARIES AnaAlgorithmLib )

# Install files from the package:
if( XAOD_STANDALONE )
   atlas_install_python_modules( python/*.py )
endif()
