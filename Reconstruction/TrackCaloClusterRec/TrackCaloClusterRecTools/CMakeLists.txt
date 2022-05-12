# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackCaloClusterRecTools )

# Component(s) in the package:
atlas_add_library( TrackCaloClusterRecToolsLib
   TrackCaloClusterRecTools/*.h src/*.cxx
   SHARED
   PUBLIC_HEADERS TrackCaloClusterRecTools
   LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel
   TrackCaloClusterRecInterfacesLib TrackVertexAssociationToolLib
   xAODTracking StoreGateLib TrkEventPrimitives xAODTruth
   xAODCaloEvent xAODPFlow xAODAssociations
   PRIVATE_LINK_LIBRARIES FourMomUtils TrkSurfaces TrkTrack
   TrkParameters TrkExInterfaces TrkCaloExtension AtlasDetDescr
   TrkParametersIdentificationHelpers xAODMuon CxxUtils)

atlas_add_component( TrackCaloClusterRecTools
   src/components/*.cxx
   LINK_LIBRARIES TrackCaloClusterRecToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
