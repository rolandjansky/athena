# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_LocRecCorrEv )

# Component(s) in the package:
atlas_add_library( ALFA_LocRecCorrEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_LocRecCorrEv
                   LINK_LIBRARIES AthContainers AthenaKernel )

atlas_add_dictionary( ALFA_LocRecCorrEvDict
                      ALFA_LocRecCorrEv/ALFA_LocRecCorrEvDict.h
                      ALFA_LocRecCorrEv/selection.xml
                      LINK_LIBRARIES ALFA_LocRecCorrEv )
