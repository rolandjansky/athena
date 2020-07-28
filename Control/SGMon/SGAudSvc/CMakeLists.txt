# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SGAudSvc )

# Component(s) in the package:
atlas_add_component( SGAudSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGAudCore GaudiKernel StoreGateLib )

# Install files from the package:
atlas_install_scripts( share/SGout2dot.py )
