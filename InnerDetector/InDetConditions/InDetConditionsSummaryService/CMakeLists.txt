# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetConditionsSummaryService )

atlas_add_library( InDetConditionsSummaryService
                   INTERFACE
                   PUBLIC_HEADERS InDetConditionsSummaryService
                   LINK_LIBRARIES GaudiKernel Identifier )
