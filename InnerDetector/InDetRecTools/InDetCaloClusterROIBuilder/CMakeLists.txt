# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCaloClusterROIBuilder )

# Component(s) in the package:
atlas_add_component( InDetCaloClusterROIBuilder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloDetDescrLib AthenaBaseComps xAODCaloEvent xAODEgamma GaudiKernel InDetRecToolInterfaces TrkSurfaces TrkCaloClusterROI TrkEventPrimitives CaloTrackingGeometryLib )
