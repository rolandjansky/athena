# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBCondRunPar )

# Component(s) in the package:
atlas_add_library( TBCondRunParLib
                   src/TBCondRunParTool.cxx
                   PUBLIC_HEADERS TBCondRunPar
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities EventInfo )

atlas_add_component( TBCondRunPar
                     src/components/*.cxx
                     LINK_LIBRARIES TBCondRunParLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

