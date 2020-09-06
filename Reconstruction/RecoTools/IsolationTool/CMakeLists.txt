# $Id: CMakeLists.txt 770003 2016-08-25 08:54:14Z krasznaa $
################################################################################
# Package: IsolationTool
################################################################################

# Declare the package name:
atlas_subdir( IsolationTool )

# Extra dependencies based on the build environment:
set( extra_lib )
set( extra_lib_private )
if( NOT XAOD_STANDALONE )
  set( extra_lib TrkParametersIdentificationHelpers CaloRecLib RecoToolInterfaces ParticlesInConeToolsLib)
  set( extra_lib_private CaloEvent CaloUtilsLib TrkCaloExtension TrkParameters TrackToCaloLib )
endif()

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( IsolationToolLib
   IsolationTool/*.h Root/*.cxx
   PUBLIC_HEADERS IsolationTool
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODBase xAODCaloEvent xAODCore xAODEgamma
   xAODEventShape xAODMuon xAODPFlow xAODPrimitives xAODTracking
   AsgDataHandlesLib InDetTrackSelectionToolLib
   IsolationCorrectionsLib
   ${extra_lib}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloGeoHelpers FourMomUtils
   ${extra_lib_private}
   )

if( NOT XAOD_STANDALONE )
  atlas_add_component( IsolationTool
    src/*.h src/*.cxx src/components/*.cxx
    LINK_LIBRARIES AthenaBaseComps GaudiKernel IsolationToolLib )
endif()
