# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MSVertexRecoAlg )

# Component(s) in the package:
atlas_add_component( MSVertexRecoAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MSVertexUtils MSVertexToolInterfaces )
