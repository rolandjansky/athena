# The name of the package:
atlas_subdir( SystematicsHandles )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   Control/AthToolSupport/AsgTools )

atlas_add_library( SystematicsHandlesLib
   SystematicsHandles/*.h SystematicsHandles/*.icc Root/*.cxx
   PUBLIC_HEADERS SystematicsHandles
   LINK_LIBRARIES AsgTools AnaAlgorithmLib PATCoreLib PATInterfaces xAODBase )

atlas_add_dictionary( SystematicsHandlesDict
   SystematicsHandles/SystematicsHandlesDict.h
   SystematicsHandles/selection.xml
   LINK_LIBRARIES SystematicsHandlesLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( SystematicsHandles
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel SystematicsHandlesLib )
endif()

add_executable (SystematicsHandles_cc_SysReadHandle test/cc_SysReadHandle.cxx)
target_link_libraries (SystematicsHandles_cc_SysReadHandle SystematicsHandlesLib)

add_executable (SystematicsHandles_cc_SysCopyHandle test/cc_SysCopyHandle.cxx)
target_link_libraries (SystematicsHandles_cc_SysCopyHandle SystematicsHandlesLib xAODEventInfo xAODJet)

add_executable (SystematicsHandles_cc_SysWriteHandle test/cc_SysWriteHandle.cxx)
target_link_libraries (SystematicsHandles_cc_SysWriteHandle SystematicsHandlesLib)

add_executable (SystematicsHandles_cc_SysListHandle test/cc_SysListHandle.cxx)
target_link_libraries (SystematicsHandles_cc_SysListHandle SystematicsHandlesLib)
