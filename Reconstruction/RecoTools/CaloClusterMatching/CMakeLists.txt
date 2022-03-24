# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloClusterMatching )

# Component(s) in the package:
atlas_add_library( CaloClusterMatchingLib
                   CaloClusterMatching/*.h
                   INTERFACE
                   PUBLIC_HEADERS CaloClusterMatching
                   LINK_LIBRARIES AsgTools AthenaBaseComps xAODCaloEvent xAODCore )

atlas_add_component( CaloClusterMatching
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel StoreGateLib CaloClusterMatchingLib )
