# $Id: CMakeLists.txt 770003 2016-08-25 08:54:14Z krasznaa $
################################################################################
# Package: IsolationTool
################################################################################

# Declare the package name:
atlas_subdir( IsolationTool )

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_deps_private )
set( extra_lib )
set( extra_lib_private )
if( NOT XAOD_STANDALONE )
  set( extra_deps Calorimeter/CaloRec Reconstruction/RecoTools/ParticlesInConeTools Tracking/TrkEvent/TrkParametersIdentificationHelpers )
  set( extra_deps_private Calorimeter/CaloEvent Calorimeter/CaloUtils Control/AthenaBaseComps Reconstruction/RecoTools/TrackToCalo Tracking/TrkEvent/TrkCaloExtension Tracking/TrkEvent/TrkParameters GaudiKernel )
  set( extra_lib TrkParametersIdentificationHelpers CaloRecLib RecoToolInterfaces )
  set( extra_lib_private CaloEvent CaloUtilsLib TrkCaloExtension TrkParameters TrackToCaloLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgDataHandles
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCore
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventShape
   Event/xAOD/xAODMuon
   Event/xAOD/xAODPFlow
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   Reconstruction/RecoTools/RecoToolInterfaces
   ${extra_deps}
   PRIVATE
   Event/FourMomUtils
   Calorimeter/CaloGeoHelpers
   ${extra_deps_private} )

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
