################################################################################
# Package: EventInfoUtils
################################################################################

# Declare the package name:
atlas_subdir( EventInfoUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Control/StoreGate
                          PRIVATE
                          Control/SGTools
                          Event/Eventinfo
                          Event/xAOD/xAODEventInfo
                          )
                          
# Libraries(s) in the package:
atlas_add_library( EventInfoUtils
                   src/*.cxx
                   PUBLIC_HEADERS EventInfoUtils
                   LINK_LIBRARIES EventInfo xAODEventInfo
                   )
