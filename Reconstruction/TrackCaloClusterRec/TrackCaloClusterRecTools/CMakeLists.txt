################################################################################
# Package: TrackCaloClusterRecTools
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecTools )

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
atlas_install_python_modules( python/*.py )
