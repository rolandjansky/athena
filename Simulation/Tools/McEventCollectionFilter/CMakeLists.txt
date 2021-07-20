# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( McEventCollectionFilter )

# Component(s) in the package:
atlas_add_component( McEventCollectionFilter
                     src/*.cxx src/components/*.cxx
                     PRIVATE_LINK_LIBRARIES AthenaBaseComps AtlasHepMCLib GaudiKernel GeneratorObjects InDetSimEvent MuonSimEvent )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
