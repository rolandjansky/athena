################################################################################
# Package: JetEDM
################################################################################

# Declare the package name:
atlas_subdir( JetEDM )

# Extra dependencies, based on the environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

## Extra dependencies, based on the build environment:
#set( extra_dep )
#set( extra_lib )
#set( extra_priv_dep )
#set( extra_priv_lib )
#if( NOT GENERATIONBASE )
#  set( extra_dep Event/xAOD/xAODTracking )
#  set( extra_lib xAODTracking )
#  set( extra_dep Event/xAOD/xAODMuon )
#  set( extra_lib xAODMuon )
#endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Event/xAOD/xAODMuon )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_library( JetEDM
                   Root/*.cxx
                   PUBLIC_HEADERS JetEDM
                   PRIVATE_INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODBase xAODJet xAODTracking
                   PRIVATE_LINK_LIBRARIES ${FASTJET_LIBRARIES} xAODMuon )

