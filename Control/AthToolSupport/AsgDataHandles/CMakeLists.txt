################################################################################
# Package: AsgDataHandles
################################################################################


# Declare the package name:
atlas_subdir( AsgDataHandles )

if( NOT XAOD_STANDALONE )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC Control/StoreGate )

# Component(s) in the package:
atlas_add_library( AsgDataHandlesLib INTERFACE
   AsgDataHandles/*.h AsgDataHandles/*.icc
   PUBLIC_HEADERS AsgDataHandles
   PRIVATE_INCLUDE_DIRS
   LINK_LIBRARIES StoreGateLib )

else()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC Control/AthToolSupport/AsgMessaging
          Control/xAODRootAccessInterfaces )

# Component(s) in the package:
atlas_add_library( AsgDataHandlesLib
   AsgDataHandles/*.h AsgDataHandles/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgDataHandles
   PRIVATE_INCLUDE_DIRS
   LINK_LIBRARIES AsgMessagingLib xAODRootAccessInterfaces )

endif()
