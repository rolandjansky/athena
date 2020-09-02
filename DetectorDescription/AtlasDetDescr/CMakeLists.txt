# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AtlasDetDescr )

# Component(s) in the package:
atlas_add_library( AtlasDetDescr
                   src/AtlasDetectorID.cxx
                   src/AtlasDetectorIDHelper.cxx
                   src/AtlasRegionHelper.cxx
                   PUBLIC_HEADERS AtlasDetDescr
                   LINK_LIBRARIES IdDict Identifier AthenaKernel
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( AtlasDetDescrDict
                      AtlasDetDescr/AtlasDetDescrDict.h
                      AtlasDetDescr/selection.xml
                      LINK_LIBRARIES AtlasDetDescr )
