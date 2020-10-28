# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetLowBetaInfo )

# Component(s) in the package:
atlas_add_library( InDetLowBetaInfo
                   src/*.cxx
                   PUBLIC_HEADERS InDetLowBetaInfo
                   LINK_LIBRARIES AthContainers AthenaKernel
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( InDetLowBetaInfoDict
                      InDetLowBetaInfo/InDetLowBetaDict.h
                      InDetLowBetaInfo/selection.xml
                      LINK_LIBRARIES InDetLowBetaInfo )
