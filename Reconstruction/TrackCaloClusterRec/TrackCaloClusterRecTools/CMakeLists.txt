################################################################################
# Package: TrackCaloClusterRecTools
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Reconstruction/TrackCaloClusterRec/TrackCaloClusterRecInterfaces                         
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Event/FourMomUtils
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODAssociations
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          )

# Component(s) in the package:
atlas_add_library( TrackCaloClusterRecToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrackCaloClusterRecTools
                   LINK_LIBRARIES GaudiKernel TrkCaloExtension TrkParametersIdentificationHelpers 
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AtlasDetDescr FourMomUtils xAODMuon xAODTracking xAODTruth 
                   xAODPFlow xAODAssociations TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack 
                   TrkExInterfaces TrackVertexAssociationToolLib TrackCaloClusterRecInterfacesLib)

atlas_add_component( TrackCaloClusterRecTools
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkCaloExtension TrkParametersIdentificationHelpers 
                     AthenaBaseComps AtlasDetDescr xAODMuon xAODTracking xAODTruth xAODPFlow xAODAssociations 
                     TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrackVertexAssociationToolLib TrackCaloClusterRecInterfacesLib
                     TrackCaloClusterRecToolsLib )
