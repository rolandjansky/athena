# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExEngine )

# Component(s) in the package:
atlas_add_component( TrkExEngine
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkVolumes TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkExUtils TrkDetDescrUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
