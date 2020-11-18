# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigAmbiguitySolver )

# Component(s) in the package:
atlas_add_component( InDetTrigAmbiguitySolver
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel CxxUtils GaudiKernel TrigInterfacesLib TrigNavigationLib TrkEventPrimitives TrkEventUtils TrkParameters TrkToolInterfaces TrkTrack )

# Install files from the package:
atlas_install_python_modules( python/*.py )
