# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IsolationAlgs )

# Component(s) in the package:
atlas_add_component( IsolationAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel CaloEvent AthContainers AthenaBaseComps StoreGateLib xAODBase xAODPrimitives xAODEgamma RecoToolInterfaces IsolationCorrectionsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

