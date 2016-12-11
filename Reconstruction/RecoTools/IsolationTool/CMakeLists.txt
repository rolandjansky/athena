# $Id: CMakeLists.txt 770003 2016-08-25 08:54:14Z krasznaa $
################################################################################
# Package: IsolationTool
################################################################################

# Declare the package name:
atlas_subdir( IsolationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloRec
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
   Reconstruction/RecoTools/ParticlesInConeTools
   Reconstruction/RecoTools/RecoToolInterfaces
   Tracking/TrkEvent/TrkParametersIdentificationHelpers
   PRIVATE
   Calorimeter/CaloEvent
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloUtils
   Control/AthenaBaseComps
   Reconstruction/RecoTools/TrackToCalo
   Tracking/TrkEvent/TrkCaloExtension
   Tracking/TrkEvent/TrkParameters
   GaudiKernel )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( IsolationToolLib
   IsolationTool/*.h Root/*.cxx
   PUBLIC_HEADERS IsolationTool
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODBase xAODCaloEvent xAODCore xAODEgamma
   xAODEventShape xAODMuon xAODPFlow xAODPrimitives xAODTracking
   InDetTrackSelectionToolLib RecoToolInterfaces
   TrkParametersIdentificationHelpers CaloRecLib IsolationCorrectionsLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CaloEvent CaloGeoHelpers
   CaloUtilsLib TrkCaloExtension TrkParameters TrackToCaloLib )

atlas_add_component( IsolationTool
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel IsolationToolLib )
