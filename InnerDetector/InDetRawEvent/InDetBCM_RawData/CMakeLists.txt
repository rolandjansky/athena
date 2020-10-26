# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetBCM_RawData )

# Component(s) in the package:
atlas_add_library( InDetBCM_RawData
                   src/*.cxx
                   PUBLIC_HEADERS InDetBCM_RawData
                   LINK_LIBRARIES AthenaKernel AthContainers )

atlas_add_dictionary( InDetBCM_RawDataDict
                      InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h
                      InDetBCM_RawData/selection.xml
                      LINK_LIBRARIES InDetBCM_RawData )
