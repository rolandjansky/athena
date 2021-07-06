# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSpacePointTool )

# Component(s) in the package:
atlas_add_library( SiSpacePointToolLib
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointTool
                   LINK_LIBRARIES AthenaBaseComps GeoPrimitives InDetPrepRawData TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES InDetIdentifier ReadoutGeometryBase InDetReadoutGeometry SiSpacePoint )

atlas_add_component( SiSpacePointTool
                     src/components/*.cxx
                     LINK_LIBRARIES SiSpacePointToolLib )
