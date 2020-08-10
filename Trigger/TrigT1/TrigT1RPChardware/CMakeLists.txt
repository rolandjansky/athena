# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1RPChardware )

# Component(s) in the package:
atlas_add_library( TrigT1RPChardwareLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPChardware
                   LINK_LIBRARIES MuonCablingTools )
