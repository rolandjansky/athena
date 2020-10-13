# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_GloRecEv )

# Component(s) in the package:
atlas_add_library( ALFA_GloRecEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_GloRecEv
                   LINK_LIBRARIES AthContainers AthenaKernel ALFA_LocRecCorrEv ALFA_LocRecEv )

atlas_add_dictionary( ALFA_GloRecEvDict
                      ALFA_GloRecEv/ALFA_GloRecEvDict.h
                      ALFA_GloRecEv/selection.xml
                      LINK_LIBRARIES ALFA_GloRecEv )
