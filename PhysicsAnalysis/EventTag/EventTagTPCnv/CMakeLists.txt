# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventTagTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( EventTagTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventTagTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib TagEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( EventTagTPCnvDict
                      EventTagTPCnv/EventTagTPCnvDict.h
                      EventTagTPCnv/selection.xml
                      LINK_LIBRARIES EventTagTPCnv )
