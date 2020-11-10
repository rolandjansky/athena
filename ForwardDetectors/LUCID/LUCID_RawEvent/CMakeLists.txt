# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_RawEvent )

# Component(s) in the package:
atlas_add_library( LUCID_RawEvent
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_RawEvent
                   LINK_LIBRARIES AthContainers AthenaKernel )

atlas_add_dictionary( LUCID_RawEventDict
                      LUCID_RawEvent/LUCID_RawEventDict.h
                      LUCID_RawEvent/selection.xml
                      LINK_LIBRARIES LUCID_RawEvent )
