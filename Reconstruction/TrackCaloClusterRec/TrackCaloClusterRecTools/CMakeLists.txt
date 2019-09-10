################################################################################
# Package: TrackCaloClusterRecTools
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Control/AthenaBaseComps
   Control/StoreGate
   Event/xAOD/xAODAssociations
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODPFlow
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   InnerDetector/InDetRecTools/TrackVertexAssociationTool
   Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecInterfaces
   Tracking/TrkEvent/TrkEventPrimitives
   PRIVATE
   Control/CxxUtils
   DetectorDescription/AtlasDetDescr
   Event/FourMomUtils
   Event/xAOD/xAODMuon
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkCaloExtension
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkParametersIdentificationHelpers
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_library( TrackCaloClusterRecToolsLib
   TrackCaloClusterRecTools/*.h src/*.cxx
   SHARED
   PUBLIC_HEADERS TrackCaloClusterRecTools
   LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODCaloEvent
   TrackCaloClusterRecInterfacesLib TrackVertexAssociationToolLib
   xAODTracking StoreGateLib TrkEventPrimitives xAODTruth
   xAODPFlow xAODAssociations
   PRIVATE_LINK_LIBRARIES FourMomUtils TrkSurfaces TrkTrack
   TrkParameters TrkExInterfaces TrkCaloExtension AtlasDetDescr
   TrkParametersIdentificationHelpers xAODMuon CxxUtils )

atlas_add_component( TrackCaloClusterRecTools
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel TrackCaloClusterRecToolsLib )
