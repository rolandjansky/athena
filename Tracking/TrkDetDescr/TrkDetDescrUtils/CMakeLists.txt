# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrUtils )

# Component(s) in the package:
atlas_add_library( TrkDetDescrUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetDescrUtils
		           LINK_LIBRARIES AthenaKernel GeoPrimitives GaudiKernel TrkEventPrimitives CxxUtils)
