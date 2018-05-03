################################################################################
# Package: RecoToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( RecoToolInterfaces )

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_lib )
#For AnalysisBase 
if( XAOD_ANALYSIS AND XAOD_STANDALONE)
#For  AthAnalysis
elseif( XAOD_ANALYSIS AND NOT XAOD_STANDALONE )
  set( extra_deps  GaudiKernel)
  set( extra_lib GaudiKernel)
#For  AthDerivation,Athena
else()
  set( extra_deps GaudiKernel Reconstruction/RecoEvent/ParticleCaloExtension Tracking/TrkEvent/TrkCaloExtension Tracking/TrkEvent/TrkTrack)
  set( extra_lib GaudiKernel ParticleCaloExtension TrkCaloExtension TrkTrack)
endif()


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
			  ${extra_deps} )

# Component(s) in the package:
atlas_add_library( RecoToolInterfaces
                   PUBLIC_HEADERS RecoToolInterfaces
		   INTERFACE
                   LINK_LIBRARIES AsgTools xAODBase xAODCaloEvent xAODPrimitives xAODTracking ${extra_lib} )


atlas_add_dictionary( RecoToolInterfacesDict
  RecoToolInterfaces/RecoToolInterfacesDict.h
  RecoToolInterfaces/selection.xml
  LINK_LIBRARIES RecoToolInterfaces)
