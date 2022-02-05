# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackToCalo )

# Component(s) in the package:
atlas_add_library( TrackToCaloLib
                   src/*.cxx
                   PUBLIC_HEADERS TrackToCalo
                   LINK_LIBRARIES CaloEvent CaloGeoHelpers CaloUtilsLib GaudiKernel GeoPrimitives InDetReadoutGeometry 
                   ParticleCaloExtension RecoToolInterfaces TrkCaloExtension TrkExInterfaces TrkParametersIdentificationHelpers 
                   TrkToolInterfaces TRT_ReadoutGeometry CaloTrackingGeometryLib xAODCaloEvent xAODTruth
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AtlasDetDescr CaloConditions CaloDetDescrLib CaloIdentifier StoreGateLib 
                   TrkDetDescrUtils TrkEventPrimitives TrkParameters TrkSurfaces TrkTrack VxVertex xAODEgamma xAODMuon xAODTracking )

atlas_add_component( TrackToCalo
                     src/components/*.cxx
                     LINK_LIBRARIES TrackToCaloLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
