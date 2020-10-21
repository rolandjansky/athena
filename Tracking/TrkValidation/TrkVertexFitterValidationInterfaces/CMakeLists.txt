# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexFitterValidationInterfaces )

atlas_add_library( TrkVertexFitterValidationInterfacesLib
                   TrkVertexFitterValidationInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkVertexFitterValidationInterfaces
                   LINK_LIBRARIES AtlasHepMCLib GaudiKernel TrkTrack TrkTruthData VxVertex )
