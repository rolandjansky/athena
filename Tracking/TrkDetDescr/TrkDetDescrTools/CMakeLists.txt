# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrTools )

# Component(s) in the package:
atlas_add_component( TrkDetDescrTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventPrimitives GaudiKernel GeoPrimitives 
                     SubDetectorEnvelopesLib TrkDetDescrInterfaces TrkDetDescrUtils 
                     TrkGeometry TrkSurfaces TrkVolumes CxxUtils)
