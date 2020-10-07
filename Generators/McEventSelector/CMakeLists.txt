# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( McEventSelector )

# Component(s) in the package:
atlas_add_component( McEventSelector
                     src/McAddress.cxx
                     src/McEventSelector.cxx
                     src/McEventCnv.cxx
                     src/McCnvSvc.cxx
                     src/McEventSelector_entries.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel EventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
