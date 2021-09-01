# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_RawEv )

# Component(s) in the package:
atlas_add_library( AFP_RawEv
                   AFP_RawEv/*.h AFP_RawEv/versions/*.h src/*.cxx
                   PUBLIC_HEADERS AFP_RawEv
                   LINK_LIBRARIES AthenaKernel AthenaBaseComps GaudiKernel )

atlas_add_dictionary( AFP_RawEvDict
                      AFP_RawEv/AFP_RawEvDict.h
                      AFP_RawEv/selection.xml
                      LINK_LIBRARIES AFP_RawEv )
