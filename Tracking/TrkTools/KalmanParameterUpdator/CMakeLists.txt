# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( KalmanParameterUpdator )

# Component(s) in the package:
atlas_add_library( KalmanParameterUpdatorLib
                   src/*.cxx
                   PUBLIC_HEADERS KalmanParameterUpdator
                   LINK_LIBRARIES EventPrimitives TrkEventPrimitives TrkParameters )
