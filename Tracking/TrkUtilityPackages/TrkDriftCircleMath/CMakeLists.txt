# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDriftCircleMath )

# Component(s) in the package:
atlas_add_library( TrkDriftCircleMath
                   src/*.cxx
                   PUBLIC_HEADERS TrkDriftCircleMath
                   LINK_LIBRARIES CxxUtils GaudiKernel )
