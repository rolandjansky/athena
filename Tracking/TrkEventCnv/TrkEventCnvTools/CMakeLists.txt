# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkEventCnvTools )

atlas_add_library( TrkEventCnvToolsLib
                   TrkEventCnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkEventCnvTools
                   LINK_LIBRARIES TrkSurfaces TrkDetDescrUtils TrkParametersBase GeoPrimitives GaudiKernel AthenaBaseComps AthenaPoolUtilities AthLinks StoreGateLib Identifier TrkPrepRawData TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( TrkEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities AtlasDetDescr GaudiKernel TrkEventCnvToolsLib TrkMeasurementBase TrkRIO_OnTrack TrkSurfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
