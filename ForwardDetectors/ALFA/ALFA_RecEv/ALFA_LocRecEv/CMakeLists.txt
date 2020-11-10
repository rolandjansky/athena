# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_LocRecEv )

# Component(s) in the package:
atlas_add_library( ALFA_LocRecEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_LocRecEv
                   LINK_LIBRARIES AthContainers AthenaKernel )

atlas_add_dictionary( ALFA_LocRecEvDict
                      ALFA_LocRecEv/ALFA_LocRecEvDict.h
                      ALFA_LocRecEv/selection.xml
                      LINK_LIBRARIES ALFA_LocRecEv )
