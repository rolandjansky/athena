# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCaloClusterROISelector )

# Component(s) in the package:
atlas_add_component( InDetCaloClusterROISelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODCaloEvent xAODEgamma CaloDetDescrLib CaloEvent InDetRecToolInterfaces CaloTrackingGeometryLib TrkCaloClusterROI egammaInterfacesLib
                     PRIVATE_LINK_LIBRARIES TrkEventPrimitives)
