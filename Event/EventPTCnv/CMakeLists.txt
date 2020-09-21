# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventPTCnv )

# Component(s) in the package:
atlas_add_library( EventPTCnv
                   src/*.cxx
                   PUBLIC_HEADERS EventPTCnv
                   LINK_LIBRARIES EventTPCnv )

atlas_add_dictionary( EventPTCnvDict
                      EventPTCnv/EventPTCnvDict.h
                      EventPTCnv/selection.xml
                      LINK_LIBRARIES EventPTCnv )
