# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RegSelSvcTest )

# Component(s) in the package:
atlas_add_component( RegSelSvcTest
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel IRegionSelector AthenaBaseComps TrigSteeringEvent TestTools )

# Install files from the package:
atlas_install_joboptions( share/*.py )
