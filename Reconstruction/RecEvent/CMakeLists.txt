# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecEvent )

# Component(s) in the package:
atlas_add_library( RecEvent
                   src/*.cxx
                   PUBLIC_HEADERS RecEvent
                   LINK_LIBRARIES SGTools AthenaKernel )

atlas_add_dictionary( RecEventDict
                      RecEvent/RecEventDict.h
                      RecEvent/selection.xml
                      LINK_LIBRARIES RecEvent )
