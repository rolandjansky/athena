# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_RawEv )

# Component(s) in the package:
atlas_add_library( AFP_RawEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_RawEv
                   LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( AFP_RawEvDict
                      AFP_RawEv/AFP_RawEvDict.h
                      AFP_RawEv/selection.xml
                      LINK_LIBRARIES AFP_RawEv )
