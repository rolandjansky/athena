# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatGeometry )

# Component(s) in the package:
atlas_add_library( iPatGeometry
                   src/DetectorIntersect.cxx
                   src/MaterialAllocator.cxx
                   src/PixelTrapezoid.cxx
                   src/SiliconDetector.cxx
                   src/SiliconDetectorBuilder.cxx
                   src/SiliconLayer.cxx
                   src/SiliconRectangle.cxx
                   src/SiliconRing.cxx
                   src/SiliconTrapezoid.cxx
                   PUBLIC_HEADERS iPatGeometry
                   LINK_LIBRARIES GeoPrimitives Identifier InDetReadoutGeometry TrkSurfaces
                   PRIVATE_LINK_LIBRARIES EventPrimitives GaudiKernel SCT_ReadoutGeometry )
