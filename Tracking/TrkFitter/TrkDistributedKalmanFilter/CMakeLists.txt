# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDistributedKalmanFilter )

# Component(s) in the package:
atlas_add_library( TrkDistributedKalmanFilterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkDistributedKalmanFilter
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldConditions TrkEventPrimitives TrkExInterfaces TrkFitterInterfaces TrkFitterUtils TrkToolInterfaces StoreGateLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr TrkDetElementBase TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack )

atlas_add_component( TrkDistributedKalmanFilter
                     src/components/*.cxx
                     LINK_LIBRARIES TrkDistributedKalmanFilterLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
