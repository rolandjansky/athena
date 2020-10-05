# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamEventPTCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( ByteStreamEventPTCnv
                         src/*.cxx
                         PUBLIC_HEADERS ByteStreamEventPTCnv
                         LINK_LIBRARIES ByteStreamEventTPCnv )

atlas_add_dictionary( ByteStreamEventPTCnvDict
                      ByteStreamEventPTCnv/ByteStreamEventPTCnvDict.h
                      ByteStreamEventPTCnv/selection.xml
                      LINK_LIBRARIES ByteStreamEventPTCnv )
