# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( ByteStreamEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS ByteStreamEventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib ByteStreamData )

atlas_add_dictionary( ByteStreamEventTPCnvDict
                      ByteStreamEventTPCnv/ByteStreamEventTPCnvDict.h
                      ByteStreamEventTPCnv/selection.xml
                      LINK_LIBRARIES ByteStreamEventTPCnv )

atlas_add_dictionary( OLD_ByteStreamEventTPCnvDict
                      ByteStreamEventTPCnv/ByteStreamEventTPCnvDict.h
                      ByteStreamEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES ByteStreamEventTPCnv )
