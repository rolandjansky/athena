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
#For AnalysisBase
if( XAOD_ANALYSIS AND XAOD_STANDALONE)
#For AthAnalysis
elseif( XAOD_ANALYSIS AND NOT XAOD_STANDALONE )
  set( extra_deps   GaudiKernel Control/AthenaBaseComps)
#For AthDerivation,Athena
else()
  set( extra_deps GaudiKernel Control/AthenaBaseComps Calorimeter/CaloRec Reconstruction/RecoTools/ParticlesInConeTools Tracking/TrkEvent/TrkParametersIdentificationHelpers )
  set( extra_deps_private Calorimeter/CaloEvent Calorimeter/CaloUtils  Reconstruction/RecoTools/TrackToCalo Tracking/TrkEvent/TrkCaloExtension Tracking/TrkEvent/TrkParameters )
  set( extra_lib TrkParametersIdentificationHelpers CaloRecLib)
  set( extra_lib_private CaloEvent CaloUtilsLib TrkCaloExtension TrkParameters TrackToCaloLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
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
   InnerDetector/InDetRecTools/TrackVertexAssociationTool
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   Reconstruction/RecoTools/RecoToolInterfaces
   ${extra_deps}
   PRIVATE
   Calorimeter/CaloGeoHelpers
   ${extra_deps_private} )

# External dependencies:
find_package (ROOT COMPONENTS Core)
find_package( Boost )

# Component(s) in the package:
atlas_add_library( IsolationToolLib
   IsolationTool/*.h Root/*.cxx
   PUBLIC_HEADERS IsolationTool
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODBase xAODCaloEvent xAODCore xAODEgamma
   xAODEventShape xAODMuon xAODPFlow xAODPrimitives xAODTracking
   InDetTrackSelectionToolLib
   TrackVertexAssociationToolLib
   IsolationCorrectionsLib
   RecoToolInterfaces	
   ${extra_lib}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloGeoHelpers
   ${extra_lib_private}
   )

if( NOT XAOD_STANDALONE )
  atlas_add_component( IsolationTool
    src/*.h src/*.cxx src/components/*.cxx
    LINK_LIBRARIES AthenaBaseComps GaudiKernel IsolationToolLib )
endif()

atlas_add_dictionary( IsolationToolDict
  IsolationTool/IsolationToolDict.h
  IsolationTool/selection.xml
  LINK_LIBRARIES IsolationToolLib)
