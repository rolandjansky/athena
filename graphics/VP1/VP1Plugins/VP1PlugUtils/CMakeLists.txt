# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1PlugUtils )

# Component(s) in the package:
atlas_add_library( VP1PlugUtils
                   src/*.c*
                   PUBLIC_HEADERS VP1PlugUtils
                   PRIVATE_LINK_LIBRARIES VP1Base VP1GeometrySystems VP1GuideLineSystems VP1PRDSystems VP1TrackSystems VP1VertexSystems )

