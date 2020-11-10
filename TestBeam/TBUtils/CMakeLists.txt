# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBUtils )

atlas_add_library( TBUtilsLib
                   TBUtils/*.h
                   INTERFACE
                   PUBLIC_HEADERS TBUtils
                   LINK_LIBRARIES TBEvent )
