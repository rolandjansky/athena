# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiZvertexTool_xk )

# Component(s) in the package:
atlas_add_component( SiZvertexTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel Identifier InDetRecToolInterfaces SiSpacePointsSeed SiSPSeededTrackFinderData VxVertex )
