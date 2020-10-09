# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTrackUtils )

# Component(s) in the package:
atlas_add_library( CaloTrackUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloTrackUtils
                   LINK_LIBRARIES GaudiKernel CaloRecLib
                   PRIVATE_LINK_LIBRARIES GeoPrimitives Particle TrkExInterfaces TrkParameters TrkParticleBase TrkSurfaces TrkTrackLink VxVertex xAODCaloEvent )

atlas_add_component( CaloTrackUtils
                     src/components/*.cxx
                     LINK_LIBRARIES CaloTrackUtilsLib )
