# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_SimEv )

# Component(s) in the package:
atlas_add_library( ALFA_SimEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_SimEv
                   LINK_LIBRARIES AthenaKernel HitManagement )

atlas_add_dictionary( ALFA_SimEvDict
                      ALFA_SimEv/ALFA_SimEvDict.h
                      ALFA_SimEv/selection.xml
                      LINK_LIBRARIES ALFA_SimEv )
