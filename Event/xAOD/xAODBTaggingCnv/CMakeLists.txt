# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODBTaggingCnv )

atlas_add_library( xAODBTaggingCnvLib
                   xAODBTaggingCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODBTaggingCnv
                   LINK_LIBRARIES GaudiKernel xAODBTagging )

# Component(s) in the package:
atlas_add_component( xAODBTaggingCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODBTaggingCnvLib xAODBTagging AthenaBaseComps AthenaKernel GaudiKernel TrigParticle )
