# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcIdentifier )

# Component(s) in the package:
atlas_add_library( ZdcIdentifier
                   src/*.cxx
                   PUBLIC_HEADERS ZdcIdentifier
                   LINK_LIBRARIES AthenaKernel AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_executable( test_zdc_id
                      test/test_zdc_id.cxx
                      LINK_LIBRARIES GaudiKernel IdDictParser Identifier ZdcIdentifier )
