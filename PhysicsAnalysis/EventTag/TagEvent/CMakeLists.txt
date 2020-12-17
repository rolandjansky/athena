# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TagEvent )

# Component(s) in the package:
atlas_add_library( TagEvent
                   TagEvent/*.h
                   INTERFACE
                   PUBLIC_HEADERS TagEvent
                   LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( TagEventDict
                      TagEvent/TagEventDict.h
                      TagEvent/selection.xml
                      LINK_LIBRARIES TagEvent )
