# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackSummary )

# Component(s) in the package:
atlas_add_library( TrkTrackSummary
                   src/*.cxx
                   PUBLIC_HEADERS TrkTrackSummary
                   LINK_LIBRARIES Identifier
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( TrkTrackSummaryDict
                      TrkTrackSummary/SummaryDict.h
                      TrkTrackSummary/selection.xml
                      LINK_LIBRARIES TrkTrackSummary )
