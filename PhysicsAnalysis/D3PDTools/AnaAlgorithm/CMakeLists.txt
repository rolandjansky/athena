# The name of the package:
atlas_subdir( AnaAlgorithm )

# Find the needed external(s):
find_package( ROOT COMPONENTS Core RIO Hist Tree TreePlayer Proof ProofPlayer )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   Control/AthToolSupport/AsgTools
   PRIVATE
   Control/xAODRootAccess )

if (XAOD_STANDALONE)
  set (headers AnaAlgorithm/*.h)
  set (inline_headers AnaAlgorithm/*.icc)
  set (sources Root/*.cxx)
else()
  set (headers AnaAlgorithm/AnaAlgorithm.h AnaAlgorithm/Global.h  AnaAlgorithm/MessageCheck.h)
  set (inline_headers AnaAlgorithm/AnaAlgorithm.icc)
  set (sources Root/AnaAlgorithm.cxx Root/MessageCheck.cxx)
endif()

atlas_add_dictionary( AnaAlgorithmDict
   AnaAlgorithm/AnaAlgorithmDict.h
   AnaAlgorithm/selection.xml
   LINK_LIBRARIES AnaAlgorithmLib )

atlas_add_library( AnaAlgorithmLib
   ${headers} ${inline_headers} ${sources}
   PUBLIC_HEADERS AnaAlgorithm
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} RootCoreUtils AsgTools xAODRootAccess )
