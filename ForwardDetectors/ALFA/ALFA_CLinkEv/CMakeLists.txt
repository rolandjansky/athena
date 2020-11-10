# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_CLinkEv )

# Component(s) in the package:
atlas_add_library( ALFA_CLinkEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_CLinkEv
                   LINK_LIBRARIES AthLinks AthenaKernel AthenaPoolUtilities ALFA_RawEv ALFA_LocRecCorrEv ALFA_LocRecEv )

atlas_add_dictionary( ALFA_CLinkEvDict
                      ALFA_CLinkEv/ALFA_CLinkEvDict.h
                      ALFA_CLinkEv/selection.xml
                      LINK_LIBRARIES ALFA_CLinkEv )
