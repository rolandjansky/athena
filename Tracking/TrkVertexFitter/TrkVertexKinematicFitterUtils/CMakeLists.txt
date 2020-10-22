# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexKinematicFitterUtils )

# Component(s) in the package:
atlas_add_library( TrkVertexKinematicFitterUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexKinematicFitterUtils
                   LINK_LIBRARIES TrkVertexFitterInterfaces )

