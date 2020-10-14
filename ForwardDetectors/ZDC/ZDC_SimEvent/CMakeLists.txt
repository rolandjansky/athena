# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZDC_SimEvent )

# Component(s) in the package:
atlas_add_library( ZDC_SimEvent
                   src/*.cxx
                   PUBLIC_HEADERS ZDC_SimEvent
                   LINK_LIBRARIES AthenaKernel HitManagement )

atlas_add_dictionary( ZDC_SimEventDict
                      ZDC_SimEvent/ZDC_SimEventDict.h
                      ZDC_SimEvent/selection.xml
                      LINK_LIBRARIES ZDC_SimEvent )
